let rec findNodeByTag =
        (GlobalTransform.{tag, nodes} as n: GlobalTransform.node(ConfigIR.kernelPlace), t) =>
  switch (tag) {
  | Some(Some(p)) when t == p => Some(n)
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

let rec animate =
        (
          flow: Flow.linear,
          n: GlobalTransform.node(ConfigIR.kernelPlace),
          next: GlobalTransform.node(ConfigIR.kernelPlace),
        )
        : GlobalTransform.node(ConfigIR.kernelPlace) => {
  let nodes = List.map(animate(flow, _, next), n.nodes);
  switch (n.tag) {
  | None => failwith("All nodes should be painted!")
  | Some(None) => {...n, nodes}
  | Some(Some(tag)) =>
    /* look up tag in flow */
    switch (List.assoc_opt(tag, flow)) {
    | None => {...n, nodes}
    | Some(destTags) =>
      /* look up dest tags in next */
      let destNodes = List.map(findNodeByTagExn(next), destTags);
      /* animate! */
      let nodeRender =
        switch (destNodes) {
        | [] => (
            bbox => {
              let renderedElem = n.nodeRender(bbox);
              <DeleteComponent renderedElem />;
            }
          )
        | _ => (
            bbox => {
              let renderedElem = n.nodeRender(bbox);
              List.map(
                (destNode: GlobalTransform.node(ConfigIR.kernelPlace)) =>
                  <TransitionComponent
                    bbox
                    renderedElem
                    /* seems like either nodeRender should have control over transform or else should remove transform arg from this */
                    transform=Transform.ident
                    nextTransform={Transform.compose(
                      destNode.globalTransform,
                      Transform.invert(n.globalTransform),
                    )}
                  />,
                destNodes,
              )
              |> Array.of_list
              |> React.array;
            }
          )
        };
      {...n, nodes, nodeRender};
    }
  };
};
