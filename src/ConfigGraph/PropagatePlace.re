/* propagates flace information

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

let convert = (flow: Flow.linearExt, n: ConfigGraphIR.node): (Flow.linearExt, ConfigGraphIR.node) => {
  let flowState = ref(flow);
  let rec convertAux = (p, nodes): list(option(ConfigGraphIR.node)) => {
    /* TODO: extFn */
    let maybePDests = List.assoc_opt(p, flowState^.pattern);
    List.mapi(
      (i, on) =>
        switch (on) {
        | None => None
        | Some(ConfigGraphIR.{place, nodes} as n) =>
          switch (place) {
          /* If the node already has a place, we're done propagating the previous place and start propagating this place */
          | [place, ..._] => Some({...n, nodes: convertAux(place, nodes)})
          | [] =>
            let place = p ++ "." ++ string_of_int(i);
            switch (maybePDests) {
            | Some(pDests) =>
              flowState :=
                {
                  ...flowState^,
                  pattern: [
                    (place, List.map(p => p ++ "." ++ string_of_int(i), pDests)),
                    ...flowState^.pattern,
                  ],
                }
            | None => ()
            };
            Some({...n, place: [place], nodes: convertAux(place, nodes)});
          }
        },
      nodes,
    );
  };
  let rec convertOption = on =>
    switch (on) {
    | None => None
    | Some(ConfigGraphIR.{place, nodes} as n) =>
      switch (place) {
      | [] => Some({...n, nodes: List.map(convertOption, nodes)})
      | [p, ..._] => Some({...n, nodes: convertAux(p, nodes)})
      }
    };
  /* sequencing for flowState mutation */
  let n = convertOption(Some(n))->Belt.Option.getExn;
  (flowState^, n);
};
