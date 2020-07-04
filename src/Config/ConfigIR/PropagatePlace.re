open ConfigIR;

/* propagates flace information

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

let convert = (flow: Flow.linear, n) => {
  let flowState = ref(flow);
  let rec convertAux = (p, nodes): list(option(ConfigIR.node)) => {
    let maybePDests = List.assoc_opt(p, flowState^);
    List.mapi(
      (i, on) =>
        switch (on) {
        | None => None
        | Some({place, nodes} as n) =>
          switch (place) {
          /* If the node already has a place, we're done propagating the previous place and start propagating this place */
          | Some(place) => Some({...n, nodes: convertAux(place, nodes)})
          | None =>
            let place = p ++ "." ++ string_of_int(i);
            switch (maybePDests) {
            | Some(pDests) =>
              flowState :=
                [(place, List.map(p => p ++ "." ++ string_of_int(i), pDests)), ...flowState^]
            | None => ()
            };
            Some({...n, place: Some(place), nodes: convertAux(place, nodes)});
          }
        },
      nodes,
    );
  };
  let rec convertOption = on =>
    switch (on) {
    | None => None
    | Some({place, nodes} as n) =>
      switch (place) {
      | None => Some({...n, nodes: List.map(convertOption, nodes)})
      | Some(p) => Some({...n, nodes: convertAux(p, nodes)})
      }
    };
  /* sequencing for flowState mutation */
  let n = convertOption(Some(n))->Belt.Option.getExn;
  (flowState^, n);
};
