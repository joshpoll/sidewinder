open ConfigIR;

let rec lowerOption = (renderHole: unit => Bobcat.KernelIR.node(ConfigIR.kernelPlace), on) =>
  switch (on) {
  | None => {...renderHole(), tag: Some(None)}
  | Some({place, nodes, renderHole, render}) =>
    let renderedNode = render(List.map(lowerOption(renderHole), nodes));
    {...renderedNode, tag: Some(place)};
  };

let lower = (n: node): Bobcat.KernelIR.node(kernelPlace) => lowerOption(n.renderHole, Some(n));
