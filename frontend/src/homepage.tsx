import DOMPurify from "isomorphic-dompurify";
import { ComponentChild, css, type FC } from "dreamland/core";
import { Marked } from "marked";
import { markedHighlight } from "marked-highlight";
import hljs from "highlight.js";
import "highlight.js/styles/stackoverflow-light.css";
import mermaid from "mermaid";

type Post = {
  name: string;
  points: number;
  author: string;
  slackDiscusion: string;
};

const marked = new Marked(
  markedHighlight({
    emptyLangClass: "hljs",
    langPrefix: "hljs language-",
    highlight(code, lang, _) {
      const language = hljs.getLanguage(lang) ? lang : "plaintext";
      return hljs.highlight(code, { language: language }).value;
    },
  }),
);

function PalatineHeader(this: FC<{ children?: ComponentChild }, {}>) {
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
`k`;
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

function IndividualVote(
  this: FC<
    {
      readonly: boolean;
      objective: string;
      participants: number;
      yes: number;
      due: Date;
    },
    {}
  >,
) {
  return (
    <div>
      <PalatineHeader>Determine an course of action</PalatineHeader>
      <div class="content">
        <div class="content-box">
          <h2 class="lato-black">{this.objective}</h2>
          <div class="progress">
            <button class="progress-button">Yes</button>
            <progress
              class="progress-bar"
              id="file"
              value={this.yes}
              max={this.participants}
            />
            <div />
            <div class="vote-count lato-black">
              {this.yes} / {this.participants}
            </div>
          </div>
          <div class="lato-bold due">
            Vote closes at {this.due.toLocaleString()}
          </div>
        </div>
      </div>
    </div>
  );
}

IndividualVote.style = css`
  .content {
    display: flex;
    justify-content: center;
    flex-direction: row;
  }
  .progress {
    display: grid;
    grid-template-columns: 20% auto;
    width: 100%;
    gap: 10px;
  }
  .progress-button {
    width: 100%;
    height: 30px;
  }
  .progress-bar {
    width: 100%;
    height: 30px;
  }
  .vote-count {
    text-align: center;
  }
  .due {
    margin: 10px;
  }
  .content-box {
    width: 70%;
  }
`;

function Voting(
  this: FC<
    {
      readonly: boolean;
      posts: Post[];
    },
    {}
  >,
) {
  return (
    <div>
      <PalatineHeader>Vote</PalatineHeader>
      <div>
        {use(this.posts).map((posts) => {
          return posts.map((post, index) => (
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
        })}
      </div>
    </div>
  );
}

Voting.style = css`
  .submit {
    margin-top: 10px;
  }
  .content {
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

function PitchBox(
  this: FC<
    {
      readonly: boolean;
      submit_disabled: boolean;
      title?: string;
      explanation?: string;
    },
    {}
  >,
) {
  this.cx.mount = () => {
    use(this.explanation).listen(() => {
      if (DOMPurify.sanitize) {
        const parsed = marked.parse(this.explanation!);
        const preview = this.root.querySelector("#preview");
        if (preview) {
          preview.innerHTML = DOMPurify.sanitize(parsed.toString());
        }
      }
    });
    this.explanation = this.explanation;
  };
  return (
    <div>
      <PalatineHeader>Pitch</PalatineHeader>
      <div class="lato-bold content">
        <form class="form">
          <div class="form-body">
            <label for="pitch-title">title</label>
            <input
              type="text"
              id="pitch-name"
              name="title"
              readonly={this.readonly}
              value={use(this.title)}
            />
            <label for="pitch-explanation">
              pitch explanation
              <br />
              (MD supported)
            </label>
            <textarea
              id="pitch-explanation"
              name="explanation"
              readonly={this.readonly}
              value={use(this.explanation)}
            ></textarea>
            <div>preview</div>
            <div id="preview" />
          </div>
          <input
            type="submit"
            value="submit"
            class="submit"
            disabled={this.readonly || this.submit_disabled}
          />
        </form>
      </div>
    </div>
  );
}
PitchBox.style = css`
  .submit {
    margin-top: 10px;
  }
  .content {
    margin: 10px;
  }
  .form-body {
    display: grid;
    grid-template-columns: 20% calc(80% - 10px);
    gap: 10px;
  }
  #pitch-explanation {
    max-width: 100%;
    resize: vertical;
  }
`;
export default function Homepage(this: FC<{}, {}>) {
  this.cx.mount = () => {
    mermaid.run();
  };
  return (
    <div>
      <div class="container">
        <div class="paragraph">
          <h1 class="title lato-black">Palatine Hill</h1>
          <div class="border" />
        </div>

        <h3 class="lato-bold paragraph">
          A HackClub competion about pitching and making interesting projects.
        </h3>

        <h3 class="lato-bold paragraph">
          What do we count as interesting? In short I like Hacker News's
          definition: "anything that gratifies one's intellectual curiosity".
          Which means you get to decide, if you find it piques your curiosity
          then it has a place here.
        </h3>

        <div class="part-table-container">
          <div class="part-table-bg">
            <div class="part-bg"></div>
            <div class="part-bg"></div>
            <div class="part-bg"></div>
            <div class="part-bg"></div>
            <div class="part-bg"></div>
          </div>
          <div class="part-table lato-bold">
            <div class="part">
              <div class="lato-black">Part 1. Sign up</div>
              Any time until June 21st 2026
            </div>
            <div class="part">
              <div class="lato-black">Part 2. Pitch & Vote </div>
              June 21st 2026 until June 28th 2026
            </div>
            <div class="part">
              <div class="lato-black">Part 3. Make your project.</div>
              When you pitch until July 18th 2026
            </div>
            <div class="part">
              <div class="lato-black">Part 4. Vote again!</div>
              July 18th 2026 to July 25th 2026
            </div>
            <div class="part">
              <div class="lato-black">Part 5. Prizes and shop open.</div>
              July 25th 2026 to August 25th 2026
            </div>
          </div>
        </div>

        <h3 class="lato-bold paragraph">
          It's fairly simple, in the first week you'll pitch an idea.
        </h3>
        <div class="embed">
          <div class="horizontal-line" />
          <div class="embed-body">
            <PitchBox
              readonly={false}
              submit_disabled={true}
              title={"Adding eval to rust"}
              explanation={
                'Embeding a minimal rust compiler into programs to dynamically compile code at runtime. The evaluated code can be used sandboxed, or interact with a state you define.\n\nThis is useful for games, instructional programs, and demos etc.\n\n```rust\nuse eval::eval;\n\nfn main() {\n\teval(\"println!(\\"Hello World!\\");");\n}\n```'
              }
            />
          </div>
        </div>

        <h3 class="lato-bold paragraph">
          Then vote on pitches based on what you find interesting.
        </h3>
        <div class="embed">
          <div class="horizontal-line" />
          <div class="embed-body">
            <Voting
              readonly={true}
              posts={[
                {
                  name: "Adding eval to Rust",
                  points: 112,
                  author: "William Daniel",
                  slackDiscusion: "about:blank",
                },
                {
                  name: "Doom in a PDF",
                  points: 97,
                  author: "vk6",
                  slackDiscusion: "about:blank",
                },
                {
                  name: "Hexecute: Launch apps by casting spells!",
                  points: 85,
                  author: "Andromeda",
                  slackDiscusion: "about:blank",
                },
                {
                  name: "Porting Celeste (2018) to the browser",
                  points: 76,
                  author: "r58",
                  slackDiscusion: "about:blank",
                },
              ]}
            />
          </div>
        </div>
        <h3 class="lato-bold paragraph">
          Every post will get added as a Slack thread for people to discuss,
          follow, and post updates.
        </h3>

        <h3 class="lato-bold paragraph">
          After the first week idea submissions will close, one day after that
          voting will close and your points will be locked in.
        </h3>

        <h3 class="lato-bold paragraph">
          Now is the time to make your project! You will be given 3 weeks with little obligations to this program 
        </h3>

        <div class="embed">
          <div class="horizontal-line" />
          <div class="embed-body">
            <IndividualVote
              readonly={true}
              objective={'Was "Adding eval to Rust" shipped?'}
              yes={23}
              participants={112}
              due={new Date(Date.now() + 24 * 60 * 60)}
            />
          </div>
        </div>

        <h3 class="lato-bold paragraph">
          Failure is a part of learning, we get that. If you would like to pivot
          at any time, you can call a vote. It's up to your voters to determine
          if your pivot or ship is as cool or cooler then the original pitch.
        </h3>

        <div class="embed">
          <div class="horizontal-line" />
          <div class="embed-body">
            <IndividualVote
              readonly={true}
              objective={
                'Can "Adding eval to Rust" pivot to "Writing a Rust superset with classes"?'
              }
              yes={45}
              participants={112}
              due={new Date(Date.now() + 24 * 60 * 60)}
            />
          </div>
        </div>

        <h3 class="lato-bold paragraph">
          In either case only a majority is needed for the decision to be made.
        </h3>

            
        <h3 class="lato-bold paragraph">
          Your final points is weighted based on how you did in three catagories.
        </h3>
        <pre class="mermaid">
          {`
pie 
    "Pitch Score" : 30
    "Project Score" : 65
    "Bonuses! Including referals." : 5
`}
        </pre>
        <br />
        <h3 class="lato-bold paragraph">
          If this sounds interesting to you, RSVP to get notifed when it
          launches.
        </h3>
        <button class="rsvp">RSVP</button>
      </div>
    </div>
  );
}
Homepage.style = css`
  .rsvp {
    width: 100%;
    height: 30px;
  }

  .title {
    width: 300px;
    margin-bottom: 0;
    font-size: xxx-large;
  }

  .embed {
    position: relative;
    margin: 10px;
  }

  .container {
    margin-top: 40px;

    color: #5c4e43;
  }
  @media screen and (width >= 800px) {
    .container {
      margin-left: calc((100vw - 70vw) / 2);
      margin-right: calc((100vw - 70vw) / 2);
    }
  }

  @media screen and (width < 800px) {
    .paragraph {
      margin: 10px;
    }
  }
  .border {
    position: absolute;
    width: 260px;
    border: 2px solid #5c4e43;
    filter: url(#squiggle);
  }

  .horizontal-line {
    position: absolute;
    transform: translate(-4px, -4px);
    width: calc(100% + 8px);
    height: calc(100% + 8px);
    background: #5c4e43;
    filter: url(#squiggle);
  }

  .embed-body {
    z-index: 1;
    position: relative;
    width: 100%;
    background: #ead8c0;
    padding-bottom: 10px;
  }

  .part-table-bg {
    position: absolute;
    transform: translate(-4px, -4px);
    width: calc(100%);
    height: calc(100%);
    background: #5c4e43;
    filter: url(#squiggle);

    display: grid;
    grid-template-columns: 20% 20% 20% 20% auto;
    gap: 4px;
    padding: 4px;
    grid-auto-flow: column;
  }

  .part-table-container {
    position: relative;
    margin: 10px;
    margin-top: 50px;
    margin-bottom: 50px;
  }

  .part-table {
    display: grid;
    grid-template-columns: 20% 20% 20% 20% auto;
    justify-content: stretch;
    grid-auto-flow: column;
    gap: 4px;
  }

  .part-bg {
    position: relative;
    background: #ead8c0;
  }

  .part {
    position: relative;
    z-index: 1;
    margin-left: 4px;
    margin-right: 4px;
  }
  .mermaid > svg {
    max-height: 40vh;
  }
`;
