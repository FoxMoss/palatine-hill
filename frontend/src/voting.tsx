import { css, type FC, ComponentChild } from "dreamland/core";
import { router } from "dreamland/router";

export type Post = {
  name: string;
  points: number;
  author: string;
  slackDiscusion: string;
  id: number | undefined;
};

export function PalatineHeader(
  this: FC<
    { clickable: boolean; children?: ComponentChild },
    { username: String; blured: boolean }
  >,
) {
  this.blured = true;
  this.cx.mount = () => {
    this.username = localStorage["name"];
    this.blured = false;
  };

  return (
    <div class="header">
      <span>
        <span>
          <span class="lato-black">Palatine Hill:</span>
          <div class="border" />
        </span>
        <span class="lato-bold title">{this.children}</span>
      </span>

      <div class="lato-regular title clickables">
        <span
          class="page-link lato-bold"
          on:click={() => {
            if (this.clickable) {
              router.navigate("dashboard");
            }
          }}
        >
          voting
        </span>
        <span
          class="page-link lato-bold"
          on:click={() => {
            if (this.clickable) {
              router.navigate("dashboard/pitch");
            }
          }}
        >
          pitch
        </span>
        <div
          style={{
            color: use(this.blured).map((val) =>
              val ? "transparent" : "black",
            ),
          }}
          class="username"
        >
          {use(this.username).and(use(this.username)).or("Johnathan Fraud")}
        </div>
      </div>
    </div>
  );
}

PalatineHeader.style = css`
  .clickables {
    display: flex;
  }
  .username {
    transition: color 0.75s cubic-bezier(0.71, 0, 0.33, 1.56) 0ms;
    min-width: 15vw;
  }

  .header {
    display: flex;
    justify-content: space-between;

    height: 30px;
    background: #f8bf7a;
    padding: 4px;
    flex-direction: row;
    align-items: center;
    transition: all 0.75s cubic-bezier(0.71, 0, 0.33, 1.56) 0ms;
  }
  .title {
    margin-left: 5px;
  }
  .border {
    position: absolute;
    width: 87px;
    border: 1px solid #5c4e43;
    filter: url(#squiggle-large);
  }
  .page-link {
    text-decoration: underline;
    margin-right: 5px;
    cursor: pointer;
  }
`;

export function Voting(
  this: FC<
    {
      readonly: boolean;
      posts: Post[] | undefined;
      loading: boolean;
    },
    {
      voted_projects: Number[];
    }
  >,
) {
  this.voted_projects = [];
  this.cx.mount = () => {
    if (!this.readonly) {
      fetch("/api/v1/my_votes", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          access_token: localStorage.getItem("access_token"),
        }),
      }).then((body) => {
        body.json().then((json: { project_id: Number }[]) => {
          this.voted_projects = json.map((project) => project.project_id);
        });
      });
    }
  };

  return (
    <div>
      <PalatineHeader clickable={this.readonly ? false : true}>
        Vote
      </PalatineHeader>
      <div
        class="posts"
        style={{
          opacity: use(this.posts).map((val) => (!val ? "0" : "1")),
        }}
      >
        {use(this.posts).map((posts) => {
          if (posts?.length != 0) {
            return posts?.map((post, index) => {
              const voted_on = use(this.voted_projects).map((projects) => {
                return projects.includes(post.id ? (post.id as number) : -1);
              });

              return (
                <div class="post">
                  <div class="lato-black place">{index + 1}.</div>
                  <div
                    class="vote"
                    style={{
                      cursor: voted_on.map((val) =>
                        val ? "initial" : "pointer",
                      ),
                    }}
                    on:click={(e: MouseEvent) => {
                      if (
                        !this.readonly &&
                        (e.target as HTMLDivElement).style.opacity != "0" &&
                        !voted_on.value
                      ) {
                        const xhr = new XMLHttpRequest();
                        xhr.open("POST", "/api/v1/upvote");
                        xhr.send(
                          `access_token=${encodeURIComponent(localStorage["access_token"])}&project_id=${post.id}`,
                        );
                        this.voted_projects.push(post.id!);

                        this.posts![index].points += 1;
                        this.posts = this.posts;
                      }
                    }}
                  >
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      viewBox="0 0 640 640"
                      style={{
                        opacity: voted_on.map((val) => (val ? "0" : "1")),
                      }}
                    >
                      {/*<!--!Font Awesome Free v7.2.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2026 Fonticons, Inc.-->*/}
                      <path d="M297.4 169.4C309.9 156.9 330.2 156.9 342.7 169.4L534.7 361.4C547.2 373.9 547.2 394.2 534.7 406.7C522.2 419.2 501.9 419.2 489.4 406.7L320 237.3L150.6 406.6C138.1 419.1 117.8 419.1 105.3 406.6C92.8 394.1 92.8 373.8 105.3 361.3L297.3 169.3z" />
                    </svg>
                  </div>
                  <div
                    class="lato-bold name"
                    on:click={() => {
                      if (post.slackDiscusion != "about:blank") {
                        window.open(
                          `https://hackclub.slack.com/archives/C0B697HHCE6/p${post.slackDiscusion?.replace(".", "")}?thread_ts=${post.slackDiscusion}&cid=C0B697HHCE6`,
                        );
                      }
                    }}
                  >
                    {post.name}
                  </div>
                  <div class="lato-regular info">
                    {post.points} points by {post.author}
                  </div>
                </div>
              );
            });
          }
          return (
            <div class="lato-regular">
              You're the first one here, so it's a little quiet. Pitches will
              apear here soon!
            </div>
          );
        })}
      </div>
    </div>
  );
}

Voting.style = css`
  .posts {
    transition: opacity 0.75s cubic-bezier(0.71, 0, 0.33, 1.56) 0ms;
  }

  .placeholder {
    padding: 10px;
    margin: 10px;
  }
  .post {
    padding: 10px;
    display: grid;

    grid-template-areas:
      "place vote name"
      "place vote info";
    grid-template-columns: 20px 30px auto;
  }
  .place {
    grid-area: place;
    font-size: large;
    display: flex;
    align-content: center;
    justify-content: center;
    flex-wrap: wrap;
  }
  .vote {
    grid-area: vote;
    display: flex;
    align-content: flex-start;
    flex-wrap: wrap;
    margin-left: 5px;
    margin-top: 5px;
    margin-right: 5px;
    cursor: pointer;
  }
  .name {
    grid-area: name;
    cursor: pointer;
  }
  .info {
    grid-area: info;
  }
`;
