type node = {
  uid: UID.t,
  nodes: list(node),
  links: list(Link.lcaPath),
  transform: Node.transform,
  bbox: Node.bbox,
  render: (list(React.element), Node.bbox, list(React.element)) => React.element,
};

/**
 *  computes the bboxes of the child nodes of the input node
 */
module MS = Belt.Map.String;
let rec computeBBoxes =
        ({uid, nodes, renderingLinks, layoutLinks, layout, computeBBox, render}: LCA.node): node => {
  let bboxList = List.map(computeBBoxes, nodes);
  let bboxMap = List.map(({uid, bbox}) => (uid, bbox), bboxList);
  let nodeTransforms = layout(bboxMap, layoutLinks);
  // Js.log2("nodeTransforms", nodeTransforms |> MS.toArray);
  let nodeBBoxes =
    MS.merge(
      bboxMap |> Array.of_list |> MS.fromArray,
      nodeTransforms,
      (uid, Some(bbox), Some(transform)) =>
      Some(bbox->Rectangle.transform(transform))
    );
  let bbox = computeBBox(nodeBBoxes);
  if (List.length(bboxList) == MS.size(nodeBBoxes)) {
    let nodes = List.map(n => {...n, transform: MS.getExn(nodeTransforms, n.uid)}, bboxList);
    {uid, nodes, links: renderingLinks, transform: Transform.ident, bbox, render};
  } else {
    Js.log("layout function doesn't preserve nodes!");
    Js.log2("bboxList", bboxList |> Array.of_list);
    Js.log2("nodeBBoxes", nodeBBoxes);
    assert(false);
  };
};