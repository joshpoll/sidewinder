/* TODO: would be nicer with option monad? */

/* let rec lowerOption = (renderHole, on) =>
   switch (on) {
   | None => renderHole
   | Some({place, nodes, render}) =>
     let renderedNode = render(List.map(lowerOption, nodes));
     {...renderedNode, tag: Some(place)};
   }; */

let layout = ((flow, n)) => {
  // let (flow, n) = propagatePlace(flow, n);
  // let n = ToKernel.lower(n);
  let (flow, n) = Paint.convert(flow, n);
  (flow, Bobcat.Kernel.layout(n));
};

/* TODO [perf]: maybe incrementalize this */
/* input: list of flows and nodes that have been propagated (and potentially transformed)
   output: list of React elements */
let compile = (~debug=false, flows, ns: list(ConfigGraphIR.node)) => {
  let ns = List.map(ToKernel.lower, ns);
  let (flows, ns) = List.combine(flows, ns) |> List.map(layout) |> List.split;
  let (flows, ns) = (flows @ [[]], ns @ [List.rev(ns) |> List.hd]);
  let nPairs = Fn.mapPairs((a, b) => (a, b), ns);
  let animatedNodes =
    Belt.List.zipBy(flows, nPairs, (flow, (n, next)) => Animate.animate(~debug, flow, n, next));
  /* TODO: maybe separate this step out? That way consumer can choose when to render and can operate used layout version for dynamic customizations. */
  List.map(Bobcat.Kernel.renderLayout, animatedNodes);
};

let compileTransition = (~debug=false, n1, flow, n2) => {
  let (n1, n2) = (ToKernel.lower(n1), ToKernel.lower(n2));
  let (flow, n1) = layout((flow, n1));
  let (_, n2) = layout(([], n2));
  let animatedNode = Animate.animate(~debug, flow, n1, n2);
  Bobcat.Kernel.renderLayout(animatedNode);
};
