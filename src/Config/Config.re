open ConfigIR;

/* lowers flow

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

let propagatePlace = (flow: Flow.linear, n) => {
  let flowState = ref(flow);
  let rec propagatePlaceAux = (p, nodes): list(option(ConfigIR.node)) => {
    let maybePDests = List.assoc_opt(p, flowState^);
    List.mapi(
      (i, on) =>
        switch (on) {
        | None => None
        | Some({place, nodes} as n) =>
          switch (place) {
          /* If the node already has a place, we're done propagating the previous place and start propagating this place */
          | Some(place) => Some({...n, nodes: propagatePlaceAux(place, nodes)})
          | None =>
            let place = p ++ "." ++ string_of_int(i);
            switch (maybePDests) {
            | Some(pDests) =>
              flowState :=
                [(place, List.map(p => p ++ "." ++ string_of_int(i), pDests)), ...flowState^]
            | None => ()
            };
            Some({...n, place: Some(place), nodes: propagatePlaceAux(place, nodes)});
          }
        },
      nodes,
    );
  };
  let rec propagatePlaceOption = on =>
    switch (on) {
    | None => None
    | Some({place, nodes} as n) =>
      switch (place) {
      | None => Some({...n, nodes: List.map(propagatePlaceOption, nodes)})
      | Some(p) => Some({...n, nodes: propagatePlaceAux(p, nodes)})
      }
    };
  /* sequencing for flowState mutation */
  let n = propagatePlaceOption(Some(n))->Belt.Option.getExn;
  (flowState^, n);
};

/* let rec lowerOption = (renderHole, on) =>
   switch (on) {
   | None => renderHole
   | Some({place, nodes, render}) =>
     let renderedNode = render(List.map(lowerOption, nodes));
     {...renderedNode, tag: Some(place)};
   }; */

let rec lowerOption = (renderHole: unit => Bobcat.KernelIR.node(ConfigIR.kernelPlace), on) =>
  switch (on) {
  | None => {...renderHole(), tag: Some(None)}
  | Some({place, nodes, renderHole, render}) =>
    let renderedNode = render(List.map(lowerOption(renderHole), nodes));
    {...renderedNode, tag: Some(place)};
  };

let lower = (n: node): Bobcat.KernelIR.node(kernelPlace) => lowerOption(n.renderHole, Some(n));

let layout = ((flow, n)) => {
  // let (flow, n) = propagatePlace(flow, n);
  let n = lower(n);
  let (flow, n) = Paint.paint(flow, n);
  (flow, Bobcat.Kernel.layout(n));
};

/* TODO [perf]: maybe incrementalize this */
/* input: list of flows and nodes that have been propagated (and potentially transformed)
   output: list of React elements */
let compile = (flows, ns) => {
  let (flows, ns) = List.combine(flows, ns) |> List.map(layout) |> List.split;
  let (flows, ns) = (flows @ [[]], ns @ [List.rev(ns) |> List.hd]);
  let nPairs = Fn.mapPairs((a, b) => (a, b), ns);
  let animatedNodes =
    Belt.List.zipBy(flows, nPairs, (flow, (n, next)) => Animate.animate(flow, n, next));
  /* TODO: maybe separate this step out? That way consumer can choose when to render and can operate used layout version for dynamic customizations. */
  List.map(Bobcat.Kernel.renderLayout, animatedNodes);
};

let compileTransition = (n1, flow, n2) => {
  let (flow, n1) = layout((flow, n1));
  let (_, n2) = layout(([], n2));
  let animatedNode = Animate.animate(flow, n1, n2);
  Bobcat.Kernel.renderLayout(animatedNode);
};
