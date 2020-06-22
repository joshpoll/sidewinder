module MS = Belt.Map.String;

let render = (n: KernelIR.node): React.element =>
  n |> LCA.fromKernel |> Layout.computeBBoxes |> RenderLinks.renderLinks |> Render.render;

let defaultRender = (nodes, links) => {
  <>
    <g className="nodes"> {nodes |> Array.of_list |> React.array} </g>
    <g className="links"> {links |> Array.of_list |> React.array} </g>
  </>;
};

let defaultLinks = links => {
  <g className="links"> {links |> Array.of_list |> React.array} </g>;
};

let defaultLayout = (bboxes, _) =>
  List.map(((uid, _)) => (uid, Transform.ident), bboxes) |> Array.of_list |> MS.fromArray;