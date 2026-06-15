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
            vote_count: Number;
          }>,
        ) => {
          // TODO calc points
          this.posts = res.map(
            (item) =>
              ({
                name: item.title,
                author: item.name,
                points: item.vote_count,
                slackDiscusion: item.pitch_timestamp,
                id: item.id
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

export function Pitch(this: FC<{}, { user: String; pitching: boolean }>) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];
  };

  this.pitching = false;

  return (
    <div class="fullscreen-container">
      <PitchBox
        readonly={false}
        submit_disabled={false}
        pitching={() => {
          this.pitching = true;
        }}
      />
      {use(this.pitching).and(<div class="pitch-cover"><div class="loading-box lato-black">loading...</div></div>)}
    </div>
  );
}
Pitch.style = css`
.pitch-cover {
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
  from {opacity: 0;}
  to {opacity: 1;}
}

.loading-box {
  background: #ead8c0;
  padding: 20px;
}
`
