/* propagates flace information

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

let convert = (flow: Flow.linearExt, n: ConfigGraphIR.node): (Flow.linearExt, ConfigGraphIR.node) => {
  let flowState = ref(flow);
  let rec convertAux = (p: ConfigGraphIR.kernelPlace, nodes): list(option(ConfigGraphIR.node)) => {
    let maybePDests =
      switch (p.pat) {
      | Some(pat) => List.assoc_opt(pat, flowState^.pattern)
      | None => None
      };
    List.mapi(
      (i, on) =>
        switch (on) {
        | None => None
        | Some(ConfigGraphIR.{place, nodes} as n) =>
          let pat =
            switch (place.pat) {
            | Some(pat) => Some(pat) /* TODO: should we just fail here?? seems to violate our assumptions... */
            | None =>
              switch (p.pat) {
              | Some(pat) =>
                /* propagate pattern flow */
                let pat = pat ++ "." ++ string_of_int(i);
                switch (maybePDests) {
                | Some(pDests) =>
                  flowState :=
                    {
                      ...flowState^,
                      pattern: [
                        (pat, List.map(p => p ++ "." ++ string_of_int(i), pDests)),
                        ...flowState^.pattern,
                      ],
                    }
                | None => ()
                };
                Some(pat);
              | _ => None
              }
            };
          let extFns = p.extFns @ place.extFns;
          let place = ConfigGraphIR.{pat, extFns};
          Some({...n, place, nodes: convertAux(place, nodes)});
        },
      nodes,
    );
  };
  /* sequencing for flowState mutation */
  let n = {...n, nodes: convertAux(n.place, n.nodes)};
  (flowState^, n);
};
