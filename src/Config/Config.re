open ConfigIR;

/* TODO: this should also modify flow.

     e.g. a |-> [c], b |-> [] becomes
     [a |-> [c], a.0 |-> [c.0], a.1 |-> [c.1]],
     [b |-> [], b.0 |-> []]

   */

/* TODO: would be nicer with option monad? */

let rec propagatePlaceAux = (p, nodes) => {
  List.mapi(
    (i, {place, nodes} as n) =>
      switch (place) {
      | Some(p') => failwith("Was propagating '" ++ p ++ "', but encountered '" ++ p' ++ "'.")
      | None =>
        let place = p ++ "." ++ string_of_int(i);
        let nodes =
          switch (nodes) {
          | None => None
          | Some(nodes) => Some(propagatePlaceAux(place, nodes))
          };
        {...n, place: Some(place), nodes};
      },
    nodes,
  );
};

let rec propagatePlace = ({place, nodes} as n) =>
  switch (nodes) {
  | None => n
  | Some(nodes) =>
    switch (place) {
    | None => {...n, nodes: Some(List.map(propagatePlace, nodes))}
    | Some(p) => {...n, nodes: Some(propagatePlaceAux(p, nodes))}
    }
  };

let rec lower = ({nodes, render}) =>
  switch (nodes) {
  | None => render(None)
  | Some(nodes) => render(Some(List.map(lower, nodes)))
  };