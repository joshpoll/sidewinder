/* map from place to parent offset and dest transforms */
type extFnTransformMap = list((Place.t, (Bobcat.Transform.t, list(Bobcat.Transform.t))));

let rec findNodeByTag =
        (Bobcat.LayoutIR.{tag, nodes} as n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace), t) =>
  switch (tag) {
  | Some({pat, extFns})
      when Belt.Option.isSome(pat) && t == Belt.Option.getExn(pat) || List.mem(t, extFns) =>
    Some(n)
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
  let links =
    List.map(
      ({lca, source, target, linkRender}: Bobcat.Link.lcaPath) => {
        Bobcat.Link.{
          lca: lca ++ "__next",
          source: List.map(uid => uid ++ "__next", source),
          target: List.map(uid => uid ++ "__next", target),
          linkRender,
        }
      },
      next.links,
    );
  switch (next.tag) {
  | None => failwith("All nodes should be painted!")
  /* we are assuming next has already been painted by flow, so this case is when the node isn't part of the flow */
  | Some({pat: None, extFns: []}) => {
      ...next,
      uid: next.uid ++ "__next",
      nodes,
      links,
      nodeRender: bbox => <AppearComponent renderedElem={next.nodeRender(bbox)} />,
    }
  | Some(_) => {...next, uid: next.uid ++ "__next", nodes, links, nodeRender: _ => React.null}
  };
};

let patRender = (~debug=false, n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace), tag, destNodes) => {
  switch (destNodes) {
  | [] =>
    if (debug) {
      Js.log2("deleting tag:", tag);
    };
    (
      bbox => {
        let renderedElem = n.nodeRender(bbox);
        <DeleteComponent key={"pat__" ++ n.uid} renderedElem />;
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
              key={"pat__" ++ n.uid ++ string_of_int(i)}
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
};

/* look up extFns in transform map and apply using Transition */
let extFnsRender =
    (n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace), extFns, eftm: extFnTransformMap, bbox) => {
  let renderedElem = n.nodeRender(bbox);
  let destTransforms =
    List.map(ef => Belt.List.getAssoc(eftm, ef, (==))->Belt.Option.getExn, extFns)
    |> List.map(((_, transforms)) => transforms)
    |> List.flatten;
  List.mapi(
    (i, destTransform: Bobcat.Transform.t) => {
      <TransitionComponent
        // Js.log2("destTransform", destTransform);
        // Js.log2("n.transform", n.transform);
        key={"extFns__" ++ n.uid ++ string_of_int(i)}
        bbox
        renderedElem
        /* seems like either nodeRender should have control over transform or else should remove transform arg from this */
        transform=Bobcat.Transform.ident
        nextTransform={Bobcat.Transform.compose(
          destTransform,
          Bobcat.Transform.invert(n.transform),
        )}
      />
    },
    destTransforms,
  )
  |> Array.of_list
  |> React.array;
};

let animate =
    (
      ~debug=false,
      flow: Flow.linearExt,
      n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace),
      next: Bobcat.LayoutIR.node(ConfigIR.kernelPlace),
    ) => {
  let rec animateAux =
          (extFnTransformMap: extFnTransformMap, n: Bobcat.LayoutIR.node(ConfigIR.kernelPlace))
          : Bobcat.LayoutIR.node(ConfigIR.kernelPlace) => {
    let eftm = ref(extFnTransformMap);
    switch (n.tag) {
    | None => failwith("All nodes should be painted!")
    | Some({extFns}) =>
      List.map(
        ef =>
          switch (Belt.List.getAssoc(eftm^, ef, (==))) {
          | Some((parentTransform, transforms)) =>
            // Js.log2("n.tag", n.tag);
            // Js.log2("n.transform", n.transform);
            // Js.log2("ef", ef);

            /* src_transform - src-child_transform = dst_transform - dst-child_transform */
            /* dst-child_transform = dst_transform - (dst_transform - dst-child_transform) = dst_transform - (src_transform - src-child_transform) */
            let deltaTransform =
              Bobcat.Transform.compose(parentTransform, Bobcat.Transform.invert(n.transform));
            let transforms =
              List.map(
                Bobcat.Transform.compose(Bobcat.Transform.invert(deltaTransform)),
                transforms,
              );
            // Js.log2("transforms after", transforms |> Array.of_list);
            eftm := Belt.List.setAssoc(eftm^, ef, (n.transform, transforms), (==));
          // Js.log2("transforms before", transforms |> Array.of_list);
          | None =>
            let extFnFlow = flow.extFn;
            /* TODO: should really look up *all* roots, but only one root for now. The change should be to replace findNodeByTagExn with something that finds every root, and also to fix extFn propagation. */
            let destNodes =
              List.map(
                findNodeByTagExn(next),
                Belt.List.getAssoc(extFnFlow, ef, (==))->Belt.Option.getExn,
              );
            let destTransforms =
              List.map((Bobcat.LayoutIR.{transform}) => transform, destNodes);
            eftm := Belt.List.setAssoc(eftm^, ef, (n.transform, destTransforms), (==));
          },
        extFns,
      )
    };
    let extFnTransformMap = eftm^;
    let nodes = List.map(animateAux(extFnTransformMap), n.nodes);
    switch (n.tag) {
    | None => failwith("All nodes should be painted!")
    | Some(place) =>
      // Js.log2("place", place);
      switch (place) {
      | {pat: None, extFns: []} =>
        if (debug) {
          Js.log2("vanish:", n.uid);
        };
        {...n, nodes, nodeRender: bbox => <VanishComponent renderedElem={n.nodeRender(bbox)} />};
      | {pat, extFns} =>
        let patRender =
          switch (pat) {
          | Some(tag) =>
            /* look up tag in flow */
            switch (List.assoc_opt(tag, flow.pattern)) {
            | None =>
              if (debug) {
                Js.log3("tag not in flow:", tag, flow.pattern |> Array.of_list);
              };
              (_bbox => React.null);
            | Some(destTags) =>
              /* look up dest tags in next */
              let destNodes = List.map(findNodeByTagExn(next), destTags);
              /* animate! */
              patRender(~debug, n, tag, destNodes);
            }
          | None => (_bbox => React.null)
          };
        let extFnsRender = extFnsRender(n, extFns, extFnTransformMap);
        let nodeRender = bbox => <g> {patRender(bbox)} {extFnsRender(bbox)} </g>;
        {...n, nodeRender, nodes};
      }
    };
    /* ---------- */
    /* ---------- */
    /* ---------- */
    // let eftm = ref(extFnTransformMap);
    // switch (n.tag) {
    // | None => failwith("All nodes should be painted!")
    // | Some({extFns}) =>
    //   List.map(
    //     ef =>
    //       switch (Belt.List.getAssoc(eftm^, ef, (==))) {
    //       | Some(transforms) =>
    //         let transforms = List.map(Bobcat.Transform.compose(n.transform), transforms);
    //         eftm := Belt.List.setAssoc(eftm^, ef, transforms, (==));
    //       | None =>
    //         let extFnFlow = flow.extFn;
    //         /* TODO: should really look up *all* roots, but only one root for now. The change should be to replace findNodeByTagExn with something that finds every root, and also to fix extFn propagation. */
    //         let destNodes =
    //           List.map(
    //             findNodeByTagExn(next),
    //             Belt.List.getAssoc(extFnFlow, ef, (==))->Belt.Option.getExn,
    //           );
    //         let destTransforms =
    //           List.map((Bobcat.LayoutIR.{transform}) => transform, destNodes);
    //         eftm := Belt.List.setAssoc(eftm^, ef, destTransforms, (==));
    //       },
    //     extFns,
    //   )
    // };
    // let extFnTransformMap = eftm^;
    // let nodes = List.map(animateAux(extFnTransformMap), n.nodes);
    // switch (n.tag) {
    // | None => failwith("All nodes should be painted!")
    // /* TODO: extFn */
    // | Some(place) =>
    //   switch (place) {
    //   | {pat: None, extFns: []} =>
    //     if (debug) {
    //       Js.log2("vanish:", n.uid);
    //     };
    //     {...n, nodes, nodeRender: bbox => <VanishComponent renderedElem={n.nodeRender(bbox)} />};
    //   | {pat, extFns} =>
    //     /* TODO: should construct a nodeRender function for pat and for extFns. then combine them at the end */
    //     let patRender =
    //       switch (pat) {
    //       | Some(tag) =>
    //         /* look up tag in flow */
    //         switch (List.assoc_opt(tag, flow.pattern)) {
    //         | None =>
    //           if (debug) {
    //             Js.log3("tag not in flow:", tag, flow.pattern |> Array.of_list);
    //           };
    //           (_bbox => React.null);
    //         | Some(destTags) =>
    //           /* look up dest tags in next */
    //           let destNodes = List.map(findNodeByTagExn(next), destTags);
    //           /* animate! */
    //           patRender(~debug, n, tag, destNodes);
    //         }
    //       | None => (_bbox => React.null)
    //       };
    //     let extFnsRender = extFnsRender(n, extFns, extFnTransformMap);
    //     let nodeRender = bbox => <g> {patRender(bbox)} {extFnsRender(bbox)} </g>;
    //     {...n, nodeRender, nodes};
    //   }
    // };
  };
  /* TODO: return a NoOp node that combines the animate node with the appearing nodes */
  let animateN = animateAux([], n);
  let appearingNodes = animateAppear(next);
  Bobcat.LayoutIR.{
    uid: "animation wrapper" ++ n.uid ++ "->" ++ next.uid,
    tag: Some(ConfigGraphIR.noPlace),
    nodes: [animateN, appearingNodes],
    links: [],
    transform: Bobcat.Transform.ident,
    bbox: Bobcat.Rectangle.union(animateN.bbox, appearingNodes.bbox),
    nodeRender: _ => React.null,
  };
};
