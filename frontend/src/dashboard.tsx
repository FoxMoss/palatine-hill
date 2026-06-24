import { css, type FC } from "dreamland/core";
import { Post, Voting } from "./voting";
import { PitchBox } from "./pitch";
import { PitchExplainer } from "./FAQ";

export function Dashboard(
  this: FC<{}, { user: String; posts: Post[] | undefined; show_help: boolean }>,
) {
  this.show_help = false;
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];

    this.show_help = !localStorage["showed_help_1"];

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
            time_created: string;
          }>,
        ) => {
          this.posts = res.map(
            (item) =>
              ({
                name: item.title,
                author: item.name,
                points: item.vote_count,
                slackDiscusion: item.pitch_timestamp,
                id: item.id,
                timeCreated: item.time_created,
              }) as Post,
          );
        },
      ),
    );
  };

  return (
    <div class="fullscreen-container">
      <Voting readonly={false} posts={use(this.posts)} loading={true} />
      {use(this.show_help).and(
        <div class="cover">
          <div class="loading-box lato-regular">
            <div class="loading-content">
              <PitchExplainer />{" "}
            </div>
            <div class="ok-container">
              <button class="ok"
                on:click={() => {
                  this.show_help = false;
                  localStorage["showed_help_1"] = true;
                }}
              >
                ok
              </button>
            </div>
          </div>
        </div>,
      )}
    </div>
  );
}
Dashboard.style = css`
  .ok-container {
    height: 30px;
    display: flex;
    justify-content: center;
    padding: 10px;
  }

  .ok {
    width: 40%;
  }


  @media screen and (width >= 800px) {
    .cover {
      padding-left: 10vw;
      padding-right: 10vw;
      padding-top: 10vh;
      padding-bottom: 10vh;
    }
  }
  .cover {
    backdrop-filter: blur(4px);
    position: absolute;
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
    height: 80vh;
  }
  .loading-content {
    height: calc(80vh - 50px);
    overflow-y: scroll;
  }
`;

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
      {use(this.pitching).and(
        <div class="pitch-cover">
          <div class="loading-box lato-black">loading...</div>
        </div>,
      )}
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
    transition: cubic-bezier(0.71, 0, 0.33, 1.56);
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
`;
