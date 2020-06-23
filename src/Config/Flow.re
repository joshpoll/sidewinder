module MS = Belt.Map.String;

/* TODO: use sets? */

type linear = list((Place.t, list(Place.t)));

/* currently unsupported */
type general = list((list(Place.t), list(Place.t)));