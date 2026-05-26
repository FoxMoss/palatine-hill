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

    fetch("/api/v1/dashboard").then((res) =>
      res.json().then(
        (
          res: Array<{
            name: String;
            slack_id: String;
            title: String;
            explanation: String;
            id: Number;
            pitch_timestamp: String;
          }>,
        ) => {
          // TODO calc points
          this.posts = res.map(
            (item) =>
              ({
                name: item.title,
                author: item.name,
                points: 0,
                slackDiscusion: item.pitch_timestamp,
              }) as Post,
          );
        },
      ),
    );
  };

  return (
    <div class="fullscreen-container">
      <Voting readonly={false} posts={use(this.posts)} loading={true} />
    </div>
  );
}
Dashboard.style = css``;

export function Pitch(this: FC<{}, { user: String }>) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];
  };

  return (
    <div class="fullscreen-container">
      <PitchBox readonly={false} submit_disabled={false} />
    </div>
  );
}
