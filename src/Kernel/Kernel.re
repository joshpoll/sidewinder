module MS = Belt.Map.String;

let render = (n: KernelIR.node): React.element =>
  n
  |> LCA.convert
  |> Layout.convert
  |> GlobalTransform.convert
  |> RenderLinks.convert
  |> Render.convert;

/* TODO: not sure where this should go */
let defaultLayout = (bboxes, _) =>
  List.map(((uid, _)) => (uid, Transform.ident), bboxes) |> Array.of_list |> MS.fromArray;