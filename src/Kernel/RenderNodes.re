type node = {
  uid: UID.t,
  nodes: list(node),
  links: list(Sidewinder.Link.lcaPath),
  /* transform relative to global frame. useful for animation */
  globalTransform: Node.transform,
  bbox: Node.bbox,
  nodeRender: Node.bbox => React.element,
};

let rec computeGlobalTransformAux =
        (globalTransform, Layout.{uid, nodes, links, transform, bbox, nodeRender}) => {
  let globalTransform = globalTransform->Transform.compose(transform);
  let nodes = List.map(computeGlobalTransformAux(globalTransform), nodes);
  {uid, nodes, links, globalTransform, bbox, nodeRender};
};

let computeGlobalTransform = computeGlobalTransformAux(Transform.init);

/* let svgTransformTransition = (transform, bbox, nextTransform, nextBBox, r) => {
     let transform = computeSVGTransform(transform, bbox);
     <g transform> r </g>;
   }; */

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
/*
 /* skips link rendering */
 let rec render = ({uid, nodes, globalTransform, bbox, nodeRender}) => {
   let transform = computeSVGTransform(globalTransform, bbox);
   <g id=uid>
     <g id={uid ++ "__node"} transform> {nodeRender(bbox)} </g>
     <g id={uid ++ "__children"}> {List.map(render, nodes) |> Array.of_list |> React.array} </g>
   </g>;
 }; */