open ConfigIR;

/* TODO: this should also modify flow.

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad */

let rec propagatePlaceAux = (p, nodes) => {
  List.mapi(
    (i, on) =>
      switch (on) {
      | None => None
      | Some({place, nodes} as n) =>
        switch (place) {
        | Some(p') => failwith("Was propagating '" ++ p ++ "', but encountered '" ++ p' ++ "'.")
        | None =>
          let place = p ++ "." ++ string_of_int(i);
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

let propagatePlace = n => propagatePlaceOption(Some(n))->Belt.Option.getExn;

let rec lowerOption = on =>
  switch (on) {
  | None => None
  | Some({nodes, render}) => Some(render(List.map(lowerOption, nodes)))
  };

let lower = n => lowerOption(Some(n))->Belt.Option.getExn;