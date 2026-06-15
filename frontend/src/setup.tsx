import { FC } from "dreamland/core";
import { PalatineHeader } from "./voting";
import { router } from "dreamland/router";

export function PageSetup(
  this: FC<{}, { hackatime_linked: "LOADING" | "YEP" | "NOPE" }>,
) {
  this.hackatime_linked = "LOADING";
  this.cx.mount = () => {
    fetch("/api/v1/about_me", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        access_token: localStorage.getItem("access_token"),
      }),
    }).then((body) =>
      body.json().then((json) => {
        this.hackatime_linked =
          json[0]["hackatime_token"] == null ? "NOPE" : "YEP";
      }),
    );
  };
  return (
    <div class="fullscreen-container">
      <PalatineHeader clickable={true}>Account Setup</PalatineHeader>

      <div class="lato-bold content">
        {use(this.hackatime_linked).map((a) => {
          if (a == "LOADING") {
            return <div>Grabbing Hackatime status....</div>;
          } else if (a == "NOPE") {
            return (
              <a
                class="lato-regular"
                href={                    `https://hackatime.hackclub.com/oauth/authorize?client_id=Kx5uVzscgb0ZlZEsWCCDLr7A03iYqbb3QQ2qCZl6Wvc&redirect_uri=${encodeURIComponent(window.location.origin + "/auth/hackatime_callback")}&response_type=code&scope=profile+read`}
              >
                Click me to link Hackatime. We need this to track your work on projects!
              </a>
            );
          } else if (a == "YEP") {
            router.navigate("/dashboard")
            return <div>Redirecting...</div>;
          }
        })}
      </div>
    </div>
  );
}
