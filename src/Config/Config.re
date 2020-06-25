open ConfigIR;

/* lowers flow

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

let propagatePlace = (flow: Flow.linear, n) => {
  let flowState = ref(flow);
  let rec propagatePlaceAux = (p, nodes) => {
    let pDests = List.assoc(p, flowState^);
    List.mapi(
      (i, on) =>
        switch (on) {
        | None => None
        | Some({place, nodes} as n) =>
          switch (place) {
          | Some(p') => failwith("Was propagating '" ++ p ++ "', but encountered '" ++ p' ++ "'.")
          | None =>
            let place = p ++ "." ++ string_of_int(i);
            flowState :=
              [(place, List.map(p => p ++ "." ++ string_of_int(i), pDests)), ...flowState^];
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
  (flowState^, propagatePlaceOption(Some(n))->Belt.Option.getExn);
};

let rec lowerOption = on =>
  switch (on) {
  | None => None
  | Some({place, nodes, render}) =>
    let renderedNode = render(List.map(lowerOption, nodes));
    Some({...renderedNode, tag: Some(place)});
  };

let lower = n => lowerOption(Some(n))->Belt.Option.getExn;

let layout = ((flow, n)) => {
  // let (flow, n) = propagatePlace(flow, n);
  let n = lower(n);
  let (flow, n) = Paint.paint(flow, n);
  (flow, Kernel.layout(n));
};

/* TODO [perf]: maybe incrementalize this */
/* input: list of flows and nodes that have been propagated (and potentially transformed)
   output: list of React elements */
let compile = (flows, ns) => {
  let (flows, ns) = List.combine(flows, ns) |> List.map(layout) |> List.split;
  let nPairs = Fn.mapPairs((a, b) => (a, b), ns);
  let animatedNodes =
    Belt.List.zipBy(flows, nPairs, (flow, (n, next)) => Animate.animate(flow, n, next));
  /* TODO: maybe separate this step out? That way consumer can choose when to render and can operate used layout version for dynamic customizations. */
  List.map(Kernel.renderLayout, animatedNodes)
  |> List.map(r =>
       <AnimationComponentProvider value=AnimationComponentHelper.{curr: Before, next: Before}>
         r
       </AnimationComponentProvider>
     );
};