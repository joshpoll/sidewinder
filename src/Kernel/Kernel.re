module MS = Belt.Map.String;

let layout = (n: KernelIR.node('a)): Layout.node('a) => n |> LCA.convert |> Layout.convert;
/* apply transitions between these */
let renderLayout = (n: Layout.node('a)): React.element =>
  n |> GlobalTransform.convert |> RenderLinks.convert |> Render.convert;

let render = (n: KernelIR.node('a)): React.element =>
  n
  |> LCA.convert
  |> Layout.convert
  |> GlobalTransform.convert
  |> RenderLinks.convert
  |> Render.convert;

/* TODO: not sure where this should go */
let defaultLayout = (bboxes, _) =>
  List.map(((uid, _)) => (uid, Transform.ident), bboxes) |> Array.of_list |> MS.fromArray;