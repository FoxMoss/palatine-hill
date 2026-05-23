import { ComponentInstance, type FC } from "dreamland/core";
import { Route, Router } from "dreamland/router";
import mermaid from "mermaid";
import { Dashboard, Page404, PageCallback, PageGenericError, PageLogin } from "./404";

mermaid.initialize({
  theme: "base",
  look: "handDrawn",
  fontFamily: "Lato",
  themeCSS: "font-weight: 700;",
});

function App(this: FC<{ url?: string }, { el: ComponentInstance<any> }>) {
  let title = "Palatine Hill";

  this.cx.mount = () => {
    let velocity = 0;
    let last_scroll = window.scrollY;
    document.addEventListener(
        "scroll", (_: Event) => {
          velocity += Math.abs(window.scrollY - last_scroll)/500;
          velocity %= 2;
          last_scroll = window.scrollY;
        });

    function frame(_ : Number) {
      

      document.getElementById("animatecomposite")
          ?.setAttribute("k2", `${Math.abs(velocity-1)}`);
      document.getElementById("animatecomposite")
          ?.setAttribute("k3", `${1-Math.abs(velocity-1)}`);

      requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);

    document.getElementById("seedanimate");
  };

  return (
    <>
      <div id="app">
        <Router initial={this.url ? [this.url, "http://127.0.0.1:5173"] : []}>
          <Route show={() => import("./homepage").then((r) => <r.default />)} />
          <Route show={() => <Page404/>} path="404"/>
          <Route show={() => <PageLogin/>} path="auth/login"/>
          <Route show={() => <PageCallback/>} path="auth/callback"/>
          <Route show={() => <PageGenericError/>} path="error"/>
          <Route show={() => <Dashboard/>} path="dashboard"/>
        </Router>
      </div>
      <>
        <title attr:innerText={title}></title>
        <meta property="og:title" content={title} />
        <link rel="icon" type="image/png" href="/logo.png" />
      </>
    </>
  );
}

export default async (url?: string) => <App url={url} />;
