/* TODO: probably needs to come after rendering nodes b/c now using a global frame so we don't know where they are? */
type node = {
  uid: UID.t,
  nodes: list(node),
  links: list(React.element),
  globalTransform: Node.transform,
  bbox: Node.bbox,
  nodeRender: Node.bbox => React.element,
};

let rec computeTransform = (node: GlobalTransform.node, path) =>
  switch (path) {
  | [] => Rectangle.transform(node.bbox, node.globalTransform)
  | [h, ...path] =>
    let node = List.find((GlobalTransform.{uid}) => h == uid, node.nodes);
    computeTransform(node, path);
  };

let renderLink = (node, Link.{source, target, linkRender}: Link.lcaPath): React.element =>
  switch (linkRender) {
  | None => <> </>
  | Some(lr) =>
    let source = computeTransform(node, source);
    let target = computeTransform(node, target);
    lr(~source, ~target);
  };

let rec convert =
        (GlobalTransform.{uid, nodes, links, globalTransform, bbox, nodeRender} as n): node => {
  let nodes = List.map(convert, nodes);
  let links = List.map(renderLink(n), links);
  {uid, nodes, links, globalTransform, bbox, nodeRender};
};