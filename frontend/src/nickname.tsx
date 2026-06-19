import { css, type FC } from "dreamland/core";
import { PalatineHeader } from "./voting";

export function NameOveride(
  this: FC<
    {},
    {
      submitting: boolean;
      access_token: string;
    }
  >,
) {
  this.submitting = false;
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.access_token = localStorage["access_token"];
  };

  return (
    <div class="fullscreen-container">
      <PalatineHeader clickable={true}>Nickname</PalatineHeader>
      <div class="lato-regular content">
        <form
          class="form"
          method="post"
          action="/api/v1/set_nickname"
          on:submit={() => {
            this.submitting = true;
          }}
        >
          <div class="form-body">
            <input
              style={{ display: "none" }}
              name="access_token"
              value={use(this.access_token)}
            />
            <label for="pitch-title">new nickname</label>
            <input
              type="text"
              id="nickname"
              name="nickname"
              maxlength="20"
              required
            />
          </div>
          <input type="submit" value="submit" class="submit" />
        </form>
      </div>
      {use(this.submitting).and(
        <div class="cover">
          <div class="loading-box lato-black">loading...</div>
        </div>,
      )}
    </div>
  );
}
NameOveride.style = css`
  .cover {
    backdrop-filter: blur(4px);
    position: absolute;
    width: 100vw;
    height: 100vh;
    top: 0;
    left: 0;
    display: flex;
    justify-content: center;
    align-items: center;
    background: rgba(0, 0, 0, 0.1);
    animation: fadein 0.75s 1;
    animation-timing-function: cubic-bezier(0.71, 0, 0.33, 1.56);
  }
  @keyframes fadein {
    from {
      opacity: 0;
    }
    to {
      opacity: 1;
    }
  }

  .loading-box {
    background: #ead8c0;
    padding: 20px;
  }
  .content {
    margin: 10px;
  }
  .form-body {
    display: grid;
    grid-template-columns: 20% calc(80% - 10px);
    gap: 10px;
  }
`;
