import { css, type FC } from "dreamland/core";
import { Post, Voting } from "./voting";
import { PitchBox } from "./pitch";

export function Dashboard(
  this: FC<{}, { user: String; posts: Post[] | undefined }>,
) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];
  };

  return (
    <div class="container">
      <Voting readonly={false} posts={use(this.posts)} loading={true} />
    </div>
  );
}
Dashboard.style = css`
  @media screen and (width >= 800px) {
    .container {
      margin-left: calc((100vw - 80vw) / 2);
      margin-right: calc((100vw - 80vw) / 2);
      margin-top: 10px;
    }
  }
`;

export function Pitch(
  this: FC<{}, { user: String; }>,
) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];
  };

  return (
    <div class="container">
      <PitchBox readonly={false}  submit_disabled={false} />
    </div>
  );
}
Pitch.style = css`
  @media screen and (width >= 800px) {
    .container {
      margin-left: calc((100vw - 80vw) / 2);
      margin-right: calc((100vw - 80vw) / 2);
      margin-top: 10px;
    }
  }
`;
