type node = {
  uid: UID.t,
  nodes: list(node),
  links: list(React.element),
  transform: Node.transform,
  /* transform relative to global frame. used for animation */
  globalTransform: Node.transform,
  bbox: Node.bbox,
  render: (list(React.element), Node.bbox, list(React.element)) => React.element,
};

let rec computeGlobalTransformAux =
        (globalTransform, RenderLinks.{uid, nodes, links, transform, bbox, render}) => {
  let globalTransform = globalTransform->Transform.compose(transform);
  let nodes = List.map(computeGlobalTransformAux(globalTransform), nodes);
  {uid, nodes, links, transform, globalTransform, bbox, render};
};

let computeGlobalTransform = computeGlobalTransformAux(Transform.init);

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

let svgTransform = (transform, bbox, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g transform> r </g>;
};

let rec render = (RenderLinks.{nodes, links, transform, bbox, render: nodeRender}) => {
  let nodes = List.map(render, nodes);
  nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox);
};

/* TODO: use react-spring */
let svgTransformTransition = (transform, bbox, nextTransform, nextBBox, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g transform> r </g>;
};

let rec findNodeByUID = (uid, {uid: candidate, nodes} as n) =>
  if (uid == candidate) {
    Some(n);
  } else {
    List.fold_left(
      (on, n) =>
        switch (on) {
        | None => findNodeByUID(uid, n)
        | Some(n) => Some(n)
        },
      None,
      nodes,
    );
  };

let findNodeByUIDExn = (uid, n) =>
  switch (findNodeByUID(uid, n)) {
  | None => failwith("couldn't find flow uid: " ++ uid)
  | Some(n) => n
  };

let rec findNodeByPathExn = (path, {nodes} as n) =>
  switch (path) {
  | [] => n
  | [i, ...path] => findNodeByPathExn(path, List.nth(nodes, i))
  };