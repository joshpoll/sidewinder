open ConfigGraphIR;

let rec lowerOption = (renderHole: unit => Bobcat.KernelIR.node(ConfigGraphIR.kernelPlace), on) =>
  switch (on) {
  | None => {...renderHole(), tag: Some(ConfigGraphIR.noPlace)}
  | Some({uid, place, nodes, links, renderHole, render}) =>
    let renderedNode = render(List.map(lowerOption(renderHole), nodes));
    {...renderedNode, uid, links, tag: Some(place)};
  };

let lower = (n: node): Bobcat.KernelIR.node(kernelPlace) => lowerOption(n.renderHole, Some(n));
