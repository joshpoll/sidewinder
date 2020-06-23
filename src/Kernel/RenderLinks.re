/* TODO: probably needs to come after rendering nodes b/c now using a global frame so we don't know where they are? */
type node = {
  uid: UID.t,
  nodes: list(node),
  links: list(React.element),
  globalTransform: Node.transform,
  bbox: Node.bbox,
  nodeRender: Node.bbox => React.element,
};

let rec computeTransform = (node: RenderNodes.node, path) =>
  switch (path) {
  | [] => Rectangle.transform(node.bbox, node.globalTransform)
  | [h, ...path] =>
    let node = List.find((RenderNodes.{uid}) => h == uid, node.nodes);
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

let rec renderLinks =
        (RenderNodes.{uid, nodes, links, globalTransform, bbox, nodeRender} as n): node => {
  let nodes = List.map(renderLinks, nodes);
  let links = List.map(renderLink(n), links);
  {uid, nodes, links, globalTransform, bbox, nodeRender};
};

/* TODO: factor out this computation so it can be shared by Render and TransitionNode */

let computeSVGTransform =
    ({translate: {x: tx, y: ty}, scale: {x: sx, y: sy}}: Node.transform, bbox) => {
  /* https://css-tricks.com/transforms-on-svg-elements/ */
  let scale =
    "translate("
    ++ Js.Float.toString(bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.)
    ++ ", "
    ++ Js.Float.toString(bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.)
    ++ ")";
  let scale = scale ++ " " ++ {j|scale($sx, $sy)|j};
  let scale =
    scale
    ++ " "
    ++ "translate("
    ++ Js.Float.toString(-. (bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.))
    ++ ", "
    ++ Js.Float.toString(-. (bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.))
    ++ ")";

  let translate = {j|translate($tx, $ty)|j};

  scale ++ " " ++ translate;
};

let svgTransform = (uid, transform, bbox, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g id={uid ++ "__node"} transform> r </g>;
};

/* Doesn't allow for late/dynamic visual changes. */
let rec render = ({uid, nodes, links, globalTransform, bbox, nodeRender}) => {
  let transform = computeSVGTransform(globalTransform, bbox);
  <g id=uid>
    <g id={uid ++ "__node"} transform> {nodeRender(bbox)} </g>
    <g id={uid ++ "__links"}> {links |> Array.of_list |> React.array} </g>
    <g id={uid ++ "__children"}> {List.map(render, nodes) |> Array.of_list |> React.array} </g>
  </g>;
};