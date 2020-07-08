/* propagates place information

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

/* input is a ConfigGraph and its flow, but all the place nodes are "pats" and none are propagated.
   output is a flow updated with pattern propagation plus places correctly sorted into pat, extFn, or both. */
/* This feels weird b/c the flow type is double encoded in the flow list and also the AST. Why is that? Feels pretty wrong to me. I think ConfigGraph should probably have list(Place.t), not this fancy thing. */

let option_of_list = l =>
  switch (l) {
  | [] => None
  | [x] => Some(x)
  | _ =>
    Js.log("list too long to convert!");
    assert(false);
  };

let list_of_option = o =>
  switch (o) {
  | None => []
  | Some(x) => [x]
  };

let convert = (flow: Flow.linearExt, n: ConfigGraphIR.node): (Flow.linearExt, ConfigGraphIR.node) => {
  let flowState = ref(flow);
  let rec convertAux =
          (ConfigGraphIR.{pat: p, extFns}, nodes: list(option(ConfigGraphIR.node))) => {
    let maybePDests =
      switch (p) {
      | Some(pat) => List.assoc_opt(pat, flowState^.pattern)
      | None => None
      };
    List.mapi(
      (i, on) =>
        switch (on) {
        | None => None
        | Some(ConfigGraphIR.{place: {extFns: [_, ..._] as extFns}}) =>
          Js.log2("extFns should be empty before propagation, but found: ", extFns);
          assert(false);
        | Some(ConfigGraphIR.{place: {pat, extFns: []}, nodes} as n) =>
          let place = pat;
          let (isPat, isExtFn) =
            switch (place) {
            | None => (false, false)
            | Some(place) => (
                List.mem_assoc(place, flowState^.pattern),
                List.mem_assoc(place, flowState^.extFn),
              )
            };
          let pat =
            switch (p, isPat) {
            | (None, _) => place
            | (Some(p), true) =>
              Js.log("Encountered " ++ place->Belt.Option.getExn ++ " when propagating " ++ p);
              assert(false);
            | (Some(p), false) =>
              /* propagate pattern flow */
              let pat = p ++ "." ++ string_of_int(i);
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
            };

          let extFns =
            if (isExtFn) {
              list_of_option(place) @ extFns;
            } else {
              extFns;
            };

          let place = ConfigGraphIR.{pat, extFns};
          Some({...n, place, nodes: convertAux(place, nodes)});
        },
      nodes,
    );
  };
  /* sequencing for flowState mutation */
  let n = convertAux(ConfigGraphIR.noPlace, [Some(n)]) |> List.hd |> Belt.Option.getExn;
  (flowState^, n);
};
