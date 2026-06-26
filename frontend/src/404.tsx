import { type FC, type ComponentChild, css } from "dreamland/core";
import { router } from "dreamland/router";

export function PageTemplate(this: FC<{ children?: ComponentChild }, {}>) {
  return (
    <div class="pagetemplate">
      <div class="container">
        <div class="paragraph">
          <h1 class="title lato-black" on:click={() => router.navigate("/")}>
            Radish Jam
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

export function PageGenericError(this: FC<{}, { error: string }>) {
  this.error = "";
  this.cx.mount = () => {
    const url_params = new URLSearchParams(window.location.search);
    if (url_params.get("error") != null) {
      this.error = url_params.get("error")!;
    }
  };
  return (
    <PageTemplate>
      <div>An error has occured, please try again.</div>
      <div class="error">{use(this.error)}</div>
    </PageTemplate>
  );
}
PageGenericError.style = css`
  .error {
    color: red;
  }
`;

export function PageLogin(this: FC<{}, {}>) {
  this.cx.mount = () => {
    window.location.assign(
      `https://auth.hackclub.com/oauth/authorize?client_id=5f0bfe15fda9940ed93fd88aa4f0c3e0&redirect_uri=${encodeURIComponent(window.location.origin + "/auth/callback")}&response_type=code&scope=name+profile+verification_status+slack_id+basic_info`,
    );
  };
  return <PageTemplate>Redirecting to login...</PageTemplate>;
}

export function PageCallback(this: FC<{}, {}>) {
  return <PageTemplate>Running up that hill...</PageTemplate>;
}
