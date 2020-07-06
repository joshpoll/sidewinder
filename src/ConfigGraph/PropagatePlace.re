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
    /* for all places in p */
    /*   look up in the pattern flow */
    /*     if there, propagate that pattern downwards */
    /*   look up in the extFn flow */
    /*     if there, ??? */
    /* combine pattern with each node's existing patterns and recurse? do I need to update that node's patterns somehow? */
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

          // switch (p, isPat, isExtFn) {
          // | (Some(p), false, false) =>
          //   /* propagate pattern flow */
          //   let pat = p ++ "." ++ string_of_int(i);
          //   switch (maybePDests) {
          //   | Some(pDests) =>
          //     flowState :=
          //       {
          //         ...flowState^,
          //         pattern: [
          //           (pat, List.map(p => p ++ "." ++ string_of_int(i), pDests)),
          //           ...flowState^.pattern,
          //         ],
          //       }
          //   | None => ()
          //   };
          //   Some(pat);
          // /* we have a pattern here. decide what to do with it */
          // | (_, Some(place)) =>
          //   /* pattern propagation */
          //   if (List.mem_assoc(place, flowState^.pattern)) {
          //     let pat =
          //       switch (p) {
          //       | None => Some(place)
          //       | Some(pPat) =>
          //         Js.log("Encountered " ++ place ++ " when propagating " ++ pPat);
          //         assert(false);
          //       };
          //     ();
          //   } else {
          //     /* this should be normal propagation. */
          //     failwith("TODO");
          //   };

          //   /* extFn propagation */
          //   if (List.mem_assoc(place, flowState^.extFn)) {
          //     failwith("TODO");
          //   };
          // };
          // let pat =
          //   switch (p, place) {
          //   | (_, [_, _, ..._]) =>
          //     Js.log2("existing place with more than one entry: ", place |> Array.of_list);
          //     assert(false);
          //   /* TODO: behavior here depends on whether the entry is an extFn or not */
          //   | (None, oPlacePat) => option_of_list(oPlacePat)
          //   | (Some(pPat), [placePat]) when List.mem_assoc(placePat, flowState^.pattern) =>
          //     Js.log("Encountered " ++ placePat ++ " when propagating " ++ pPat);
          //     assert(false);
          //   /* extFn case */
          //   | (Some(pPat), [extFn]) when List.mem_assoc(extFn, flowState^.extFn) =>
          //     /* TODO: add extFn to extFn list */
          //     assert(false)
          //   | (Some(pPat), [place]) =>
          //     Js.log("Unused place " ++ place);
          //     assert(false);
          //   | (Some(pPat), []) =>
          //     /* propagate pattern flow */
          //     let pat = pPat ++ "." ++ string_of_int(i);
          //     switch (maybePDests) {
          //     | Some(pDests) =>
          //       flowState :=
          //         {
          //           ...flowState^,
          //           pattern: [
          //             (pat, List.map(p => p ++ "." ++ string_of_int(i), pDests)),
          //             ...flowState^.pattern,
          //           ],
          //         }
          //     | None => ()
          //     };
          //     Some(pat);
          //   };
          // let extFns = p.extFns @ place.extFns;
          // let place = ConfigGraphIR.{pat, extFns};
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
