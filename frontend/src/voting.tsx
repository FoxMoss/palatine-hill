
import { css, type FC, ComponentChild } from "dreamland/core";

export type Post = {
  name: string;
  points: number;
  author: string;
  slackDiscusion: string;
};
export function PalatineHeader(this: FC<{ children?: ComponentChild }, {}>) {
  return (
    <div class="header">
      <div>
        <div class="lato-black">Palatine Hill:</div>
        <div class="border" />
      </div>
      <div class="lato-bold title">{this.children}</div>
    </div>
  );
}

PalatineHeader.style = css`
  .header {
    display: flex;
    height: 30px;
    background: #f8bf7a;
    padding: 4px;
    flex-direction: row;
    align-items: center;
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
`;

export function Voting(
  this: FC<
    {
      readonly: boolean;
      posts: Post[] | undefined;
      loading: boolean;
    },
    {}
  >,
) {
  return (
    <div>
      <PalatineHeader>Vote</PalatineHeader>
      <div>
        {use(this.posts)
          .map((posts) => {
            return posts?.map((post, index) => (
              <div class="post">
                <div class="lato-black place">{index + 1}.</div>
                <div class="vote">
                  <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 640 640">
                    {/*<!--!Font Awesome Free v7.2.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2026 Fonticons, Inc.-->*/}
                    <path d="M297.4 169.4C309.9 156.9 330.2 156.9 342.7 169.4L534.7 361.4C547.2 373.9 547.2 394.2 534.7 406.7C522.2 419.2 501.9 419.2 489.4 406.7L320 237.3L150.6 406.6C138.1 419.1 117.8 419.1 105.3 406.6C92.8 394.1 92.8 373.8 105.3 361.3L297.3 169.3z" />
                  </svg>
                </div>
                <div class="lato-bold name">{post.name}</div>
                <div class="lato-regular info">
                  {post.points} points by {post.author}
                </div>
              </div>
            ));
          })
          .or(
            <div class="placeholder lato-bold">
              {use(this.loading)
                .and("Loading...")
                .or("Hi! You're the first person here, mind the quietness.")}
            </div>,
          )}
      </div>
    </div>
  );
}

Voting.style = css`
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
  }
  .info {
    grid-area: info;
  }
`;
