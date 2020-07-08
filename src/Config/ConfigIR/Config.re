/* TODO: would be nicer with option monad? */

let layout = ((flow, n)) => {
  let (flow, n) = Paint.convert(flow, n);
  (flow, Bobcat.Kernel.layout(n));
};

/* TODO [perf]: maybe incrementalize this */
/* input: list of flows and nodes that have been propagated (and potentially transformed)
   output: list of React elements */
let compile = (~debug=false, flows, ns: list(ConfigGraphIR.node)) => {
  let ns = List.map(ToKernel.lower, ns);
  let (flows, ns) = List.combine(flows, ns) |> List.map(layout) |> List.split;
  let (flows, ns) = (flows @ [Flow.none], ns @ [List.rev(ns) |> List.hd]);
  let nPairs = Fn.mapPairs((a, b) => (a, b), ns);
  let animatedNodes =
    Belt.List.zipBy(flows, nPairs, (flow, (n, next)) => Animate.animate(~debug, flow, n, next));
  /* TODO: maybe separate this step out? That way consumer can choose when to render and can operate used layout version for dynamic customizations. */
  List.map(Bobcat.Kernel.renderLayout, animatedNodes);
};

let compileTransition = (~debug=false, n1, flow, n2) => {
  let (n1, n2) = (ToKernel.lower(n1), ToKernel.lower(n2));
  let (flow, n1) = layout((flow, n1));
  let (_, n2) = layout((Flow.none, n2));
  let animatedNode = Animate.animate(~debug, flow, n1, n2);
  Bobcat.Kernel.renderLayout(animatedNode);
};

let filterPlaces = (places: list(Place.t), n: ConfigIR.node) => {
  let rec filterPlacesOption = (on: option(ConfigIR.node)) =>
    switch (on) {
    | None => None
    | Some(n) =>
      let nodes = List.map(filterPlacesOption, n.nodes);
      switch (n.place) {
      | Some(p) when !List.mem(p, places) => Some({...n, nodes, place: None})
      | _ => Some({...n, nodes})
      };
    };
  filterPlacesOption(Some(n))->Belt.Option.getExn;
};

let liftToCompiledTransition = n => {
  let (_, n) =
    n |> filterPlaces([]) |> ToConfigGraph.lower |> PropagatePlace.convert(Sidewinder.Flow.none);
  compileTransition(n, Flow.none, n);
};
