# Rewrite of Sidewinder

Because every prototype needs a ground-up rewrite every once in a while.

Adds a second IR: Config.

AST -> Config -> Kernel. Config is a convenient representation for abstract machine ASTs based on
the formalism we are developing for them. It allows us to apply simple data transformations and
compute transition flow easily. Importantly, configuration nodes map to possibly many kernel nodes
and must gracefully except some of their children being None. This allows configuration nodes to
move around in a tree structure, while ensuring they're renderable. Computing flow information on
the configuration is much simpler, and makes the mapping down of flow more explicit.

Customization. Transformations on the Config AST introduce the first useful data customization
option in Sidewinder. The motivating example is needing to convert a zipper structure from a focus
and a list of contexts back into a nested structure. The features described in the previous
paragraph facilitate this transformation.

Config interface. Input Config ASTs DO NOT have links and ONLY have symbolic place information (used
for flow transitions, map to pattern match variables) at leaves. Transformations on a Config can
introduce these later. Constraining input Configs allows us to map their theory easily onto abstract
machine formalisms. We therefore do not anticipate these constraints to limit expressiveness,
because they are just as expressive as the general abstract machine definition.

The flow interface is TBD, however it will be external to the Config AST structure. They are
connected by the flow interface referring to symbolic places within the AST.

The notion of symbolic place is loosely inspired by Oxide's notion of symbolic place,
https://arxiv.org/pdf/1903.00982.pdf. Really, places are meant to refer to pattern match variables,
but we think "variable" might be more confusing. The naming and utility of places or other ways of
identifying structures in the Config AST will probably become clear as we flesh out V2.

# ReasonReact Template & Examples

This is:
- A template for your new ReasonReact project.
- A collection of thin examples illustrating ReasonReact usage.
- Extra helper documentation for ReasonReact (full ReasonReact docs [here](https://reasonml.github.io/reason-react/)).

`src` contains 4 sub-folders, each an independent, self-contained ReasonReact example. Feel free to delete any of them and shape this into your project! This template's more malleable than you might be used to =).

The point of this template and examples is to let you understand and personally tweak the entirely of it. We **don't** give you an opaque, elaborate mega build setup just to put some boxes on the screen. It strikes to stay transparent, learnable, and simple. You're encouraged to read every file; it's a great feeling, having the full picture of what you're using and being able to touch any part.

## Run

```sh
npm install
npm run server
# in a new tab
npm start
```

Open a new web page to `http://localhost:8000/`. Change any `.re` file in `src` to see the page auto-reload. **You don't need any bundler when you're developing**!

**How come we don't need any bundler during development**? We highly encourage you to open up `index.html` to check for yourself!

# Features Used

|                           | Blinking Greeting | Reducer from ReactJS Docs | Fetch Dog Pictures | Reason Using JS Using Reason |
|---------------------------|-------------------|---------------------------|--------------------|------------------------------|
| No props                  |                   | ✓                         |                    |                              |
| Has props                 |                   |                           |                    | ✓                            |
| Children props            | ✓                 |                           |                    |                              |
| No state                  |                   |                           |                    | ✓                            |
| Has state                 | ✓                 |                           |  ✓                 |                              |
| Has state with useReducer |                   | ✓                         |                    |                              |
| ReasonReact using ReactJS |                   |                           |                    | ✓                            |
| ReactJS using ReasonReact |                   |                           |                    | ✓                            |
| useEffect                 | ✓                 |                           |  ✓                 |                              |
| Dom attribute             | ✓                 | ✓                         |                    | ✓                            |
| Styling                   | ✓                 | ✓                         |  ✓                 | ✓                            |
| React.array               |                   |                           |  ✓                 |                              |

# Bundle for Production

We've included a convenience `UNUSED_webpack.config.js`, in case you want to ship your project to production. You can rename and/or remove that in favor of other bundlers, e.g. Rollup.

We've also provided a barebone `indexProduction.html`, to serve your bundle.

```sh
npm install webpack webpack-cli
# rename file
mv UNUSED_webpack.config.js webpack.config.js
# call webpack to bundle for production
./node_modules/.bin/webpack
open indexProduction.html
```

# Handle Routing Yourself

To serve the files, this template uses a minimal dependency called `moduleserve`. A URL such as `localhost:8000/scores/john` resolves to the file `scores/john.html`. If you'd like to override this and handle URL resolution yourself, change the `server` command in `package.json` from `moduleserve ./ --port 8000` to `moduleserve ./ --port 8000 --spa` (for "single page application"). This will make `moduleserve` serve the default `index.html` for any URL. Since `index.html` loads `Index.bs.js`, you can grab hold of the URL in the corresponding `Index.re` and do whatever you want.

By the way, ReasonReact comes with a small [router](https://reasonml.github.io/reason-react/docs/en/router) you might be interested in.
