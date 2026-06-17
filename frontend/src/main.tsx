import { ComponentInstance, type FC } from "dreamland/core";
import { Route, Router } from "dreamland/router";
import mermaid from "mermaid";
import { Page404, PageCallback, PageGenericError, PageLogin } from "./404";
import { Dashboard, Pitch } from "./dashboard";
import { PageSetup } from "./setup";

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
          <Route show={() => <PageSetup/>} path="setup"/>
          <Route show={() => <PageCallback/>} path="auth/callback"/>
          <Route show={() => <PageGenericError/>} path="error"/>
          <Route show={() => <Dashboard/>} path="dashboard"/>
          <Route show={() => <Pitch/>} path="dashboard/pitch"/>
        </Router>
      </div>
      <>
        <title attr:innerText={title}></title>
        <meta property="og:title" content={title} />
        <link rel="icon" type="image/png" href="/logo.png" />

          {/* Open Graph */}
          <meta property="og:title" content={title} />
          <meta property="og:description" content={"Palatine Hill is a software jam for teens about making creative, novel, and technical projects."} />
          <meta property="og:type" content="website" />
          <meta property="og:image" content="/logo.png" />
          <meta property="og:image:width" content="630" />
          <meta property="og:image:height" content="630" />
          <meta property="og:site_name" content={"Palatine Hill"} />

          {/* Twitter Card */}
          <meta name="twitter:card" content="summary_large_image" />
          <meta name="twitter:title" content={title} />
          <meta name="twitter:description" content={"Palatine Hill is a software jam for teens about making creative, novel, and technical projects."} />
          <meta name="twitter:image" content={"/logo.png"} />
      </>
    </>
  );
}

export default async (url?: string) => <App url={url} />;
