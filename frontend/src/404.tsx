import { type FC, type ComponentChild } from "dreamland/core";
import { router } from "dreamland/router";

export function PageTemplate(this: FC<{ children?: ComponentChild }, {}>) {
  return (
    <div class="pagetemplate">
      <div class="container">
        <div class="paragraph">
          <h1 class="title lato-black" on:click={() => router.navigate("/")}>
            Palatine Hill
          </h1>
          <div class="border" />
        </div>
        <h3 class="lato-bold paragraph">{this.children}</h3>
      </div>
    </div>
  );
}

export function Page404(this: FC<{}, {}>) {
  return <PageTemplate>404, Hill not found.</PageTemplate>;
}

export function PageGenericError(this: FC<{}, {}>) {
  return <PageTemplate>An error has occured, please try again.</PageTemplate>;
}

export function PageLogin(this: FC<{}, {}>) {
  this.cx.mount = () => {
    window.location.assign(
      `https://auth.hackclub.com/oauth/authorize?client_id=5f0bfe15fda9940ed93fd88aa4f0c3e0&redirect_uri=${encodeURIComponent(window.location.origin + "/auth/callback")}&response_type=code&scope=name+profile+verification_status+slack_id`,
    );
  };
  return <PageTemplate>Redirecting to login...</PageTemplate>;
}

export function PageCallback(this: FC<{}, {}>) {
  return <PageTemplate>Running up that hill...</PageTemplate>;
}

export function Dashboard(this: FC<{}, { user: String }>) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];
  };

  return (
    <PageTemplate>
      Logged in as{" "}
      <span>
        {use(this.user)
          .and((val) => val)
          .or("...")}
      </span>
    </PageTemplate>
  );
}
