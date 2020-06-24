/* type node = {
     uid: UID.t,
     nodes: list(node),
     links: list(React.element),
     transform: Node.transform,
     /* transform relative to global frame. used for animation */
     globalTransform: Node.transform,
     bbox: Node.bbox,
     renderNode: (Node.bbox, list(React.element)) => React.element,
     render: (UID.t, list(React.element), path, list(path), Node.bbox, Node.transform, React.element) => React.element
   }; */

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

let render = (uid, nodes, ownPath, paths, bbox, transform, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g id=uid>
    <g id={uid ++ "__node"} transform> r </g>
    <g id={uid ++ "__children"}> {nodes |> Array.of_list |> React.array} </g>
  </g>;
};