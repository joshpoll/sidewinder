let render = (n: KernelIR.node) =>
  n |> LCA.fromKernel |> Layout.computeBBoxes |> RenderLinks.renderLinks |> Render.render;