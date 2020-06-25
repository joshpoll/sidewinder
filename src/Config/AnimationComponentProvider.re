/* https://dev.to/margaretkrutikova/reason-react-context-explained-in-action-5eki */

let make = React.Context.provider(AnimationComponentHelper.context);

/* Tell bucklescript how to translate props into JS */
let makeProps = (~value, ~children, ()) => {"value": value, "children": children};