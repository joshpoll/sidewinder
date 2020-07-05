let rec findNodeByTag =
        (Bobcat.LayoutIR.{tag, nodes} as n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace), t) =>
  switch (tag) {
  | Some({pat: Some(p)}) when t == p => Some(n)
  | _ =>
    List.fold_left(
      (on, n) =>
        switch (on) {
        | None => findNodeByTag(n, t)
        | Some(n) => Some(n)
        },
      None,
      nodes,
    )
  };

let findNodeByTagExn = (n, t) =>
  switch (findNodeByTag(n, t)) {
  | None =>
    Js.log2("failure while searching", n);
    failwith("couldn't find flowTag: " ++ t);
  | Some(n) => n
  };

let rec animateAppear = (next: Bobcat.LayoutIR.node(ConfigIR.kernelPlace)) => {
  let nodes = List.map(animateAppear, next.nodes);
  switch (next.tag) {
  | None => failwith("All nodes should be painted!")
  /* we are assuming next has already been painted by flow, so this case is when the node isn't part of the flow */
  | Some({pat: None, extFns: []}) => {
      ...next,
      uid: next.uid ++ "__next",
      nodes,
      nodeRender: bbox => <AppearComponent renderedElem={next.nodeRender(bbox)} />,
    }
  | Some(_) => {...next, uid: next.uid ++ "__next", nodes, nodeRender: _ => React.null}
  };
};

let animate =
    (
      ~debug=false,
      flow: Flow.linearExt,
      n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace),
      next: Bobcat.LayoutIR.node(ConfigIR.kernelPlace),
    ) => {
  let rec animateAux =
          (n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace))
          : Bobcat.LayoutIR.node(ConfigIR.kernelPlace) => {
    let nodes = List.map(animateAux, n.nodes);
    switch (n.tag) {
    | None => failwith("All nodes should be painted!")
    /* TODO: extFn */
    | Some({pat: None}) =>
      if (debug) {
        Js.log2("vanish:", n.uid);
      };
      {...n, nodes, nodeRender: bbox => <VanishComponent renderedElem={n.nodeRender(bbox)} />};
    | Some({pat: Some(tag)}) =>
      /* look up tag in flow */
      switch (List.assoc_opt(tag, flow.pattern)) {
      | None =>
        if (debug) {
          Js.log3("tag not in flow:", tag, flow.pattern |> Array.of_list);
        };
        {...n, nodes};
      | Some(destTags) =>
        /* look up dest tags in next */
        let destNodes = List.map(findNodeByTagExn(next), destTags);
        /* animate! */
        let nodeRender =
          switch (destNodes) {
          | [] =>
            if (debug) {
              Js.log2("deleting tag:", tag);
            };
            (
              bbox => {
                let renderedElem = n.nodeRender(bbox);
                <DeleteComponent renderedElem />;
              }
            );
          | _ =>
            if (debug) {
              Js.log2("persisting or copying tag:", tag);
            };
            (
              bbox => {
                let renderedElem = n.nodeRender(bbox);
                List.mapi(
                  (i, destNode: Bobcat.LayoutIR.node(ConfigIR.kernelPlace)) =>
                    <TransitionComponent
                      key={n.uid ++ string_of_int(i)}
                      bbox
                      renderedElem
                      /* seems like either nodeRender should have control over transform or else should remove transform arg from this */
                      transform=Bobcat.Transform.ident
                      nextTransform={Bobcat.Transform.compose(
                        destNode.transform,
                        Bobcat.Transform.invert(n.transform),
                      )}
                    />,
                  destNodes,
                )
                |> Array.of_list
                |> React.array;
              }
            );
          };
        {...n, nodes, nodeRender};
      }
    };
  };
  /* TODO: return a NoOp node that combines the animate node with the appearing nodes */
  let animateN = animateAux(n);
  let appearingNodes = animateAppear(next);
  Bobcat.LayoutIR.{
    uid: "animation wrapper" ++ n.uid ++ "->" ++ next.uid,
    tag: Some(ConfigGraphIR.{pat: None, extFns: []}),
    nodes: [animateN, appearingNodes],
    links: [],
    transform: Bobcat.Transform.ident,
    bbox: Bobcat.Rectangle.union(animateN.bbox, appearingNodes.bbox),
    nodeRender: _ => React.null,
  };
};
