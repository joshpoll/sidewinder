open ConfigGraphIR;

let rec lowerOption = (renderHole: unit => Bobcat.KernelIR.node(ConfigGraphIR.kernelPlace), on) =>
  switch (on) {
  | None => {...renderHole(), tag: Some({pat: None, extFns: []})}
  | Some({place, nodes, renderHole, render}) =>
    let renderedNode = render(List.map(lowerOption(renderHole), nodes));
    {...renderedNode, tag: Some(place)};
  };

let lower = (n: node): Bobcat.KernelIR.node(kernelPlace) => lowerOption(n.renderHole, Some(n));
