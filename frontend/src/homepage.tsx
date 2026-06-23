import { css, type FC } from "dreamland/core";
import "highlight.js/styles/stackoverflow-light.css";
import { PalatineHeader, Voting } from "./voting";
import { PitchBox } from "./pitch";

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
      <PalatineHeader clickable={this.readonly ? false : true}>
        Determine an course of action
      </PalatineHeader>
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

export default function Homepage(this: FC<{}, {}>) {
  return (
    <div>
      <div class="container">
        <div class="paragraph">
          <h1 class="title lato-black">Radish Jam</h1>
          <div class="border" />
        </div>

        <h3 class="lato-regular paragraph copy">
          No entry fee. Full creative control. A global software jam for teens.
        </h3>

        <h3 class="lato-bold paragraph big-text">The Prizes</h3>

        <div class="prizes lato-regular">
          <div />
          <span class="prize-label">First place:</span>
          <span class="prize-item">
            <img class="prize-img" src="/f16.webp" />
            Framework Laptop 16
          </span>
          <div />
        </div>
        <div class="prizes lato-regular">
          <span class="prize-label">Second place:</span>
          <span class="prize-item">
            <img class="prize-img" src="/f13.webp" />
            Framework Laptop 13
          </span>
          <span class="prize-label">Third place:</span>
          <span class="prize-item">
            <img class="prize-img" src="/flipper.webp" />A Flipper Zero
          </span>
          <span class="prize-label">Best pitch:</span>
          <span class="prize-item">
            <img class="prize-img" src="/zero2.webp" />
            Raspberry Pi Zero 2 W
          </span>
          <span class="prize-label">All participants!</span>
          <span class="prize-item">
            <img class="prize-img" src="/heart.webp" />A secret, but physical
            token of my apretiation!
          </span>
        </div>
        <div class="prizes lato-regular">
        </div>
        <div class="lato-regular small-text">
          This is given total hours shipped reaches 365. Prizes might be
          adjusted for the budget.
        </div>

        <div class="lato-bold paragraph">What's a software jam?</div>
        <div class="lato-regular paragraph">
          A software jam is a competition in which developers battle it out to
          make the coolest, most innovative projects with the winners being
          decided apon by a vote.
        </div>

        <div class="lato-bold paragraph">Who can participate?</div>
        <div class="lato-regular paragraph">
          Anyone 18 or below, regardless of area of expertise and location.
        </div>

        <div class="lato-bold paragraph">Why participate?</div>
        <div class="lato-regular paragraph">
          This is your time to compete, to make something your proud of, to
          create something that matters <span class="lato-bold">to you</span>. Why
          spend your summer playing games, doomscrolling social feeds, when you
          could be out there battling and honing your skill. When you care about
          your work, others will too.
        </div>

        <div class="lato-bold paragraph">Rough outline</div>

        <div class="lato-regular paragraph">
          Running from June 22nd to July 18th.
        </div>

        <div class="part-table-container">
          <div class="part-table-bg">
            <div class="part-bg"></div>
            <div class="part-bg"></div>
            <div class="part-bg"></div>
            <div class="part-bg"></div>
          </div>
          <div class="part-table lato-bold">
            <div class="part">
              <div class="lato-black">Part 1. Pitch your ideas</div>
            </div>
            <div class="part">
              <div class="lato-black">Part 2. Make your project</div>
              It <b>doesn't need to be approved</b>, just as long as you pitch
              your idea. You can make it :{")"}
            </div>
            <div class="part">
              <div class="lato-black">
                Part 3. Vote on your favorite projects
              </div>
            </div>
            <div class="part">
              <div class="lato-black">
                Part 4. Prizes are given out for the highest scoring projects
              </div>
            </div>
          </div>
        </div>

        <div class="rsvp-container">
          <input placeholder="name@domain.tld" />
          <button
            class="rsvp"
            on:click={() => window.location.assign("/auth/login")}
          >
            Register an account!
          </button>
        </div>
        <h3 class="lato-bold paragraph">What's a pitch?</h3>

        <div class="lato-regular paragraph">
          A pitch is a short summary of what you want to make, nothing set it
          stone.{" "}
          <div class="lato-bold">
            Once you pitch you can get started right away.
          </div>
        </div>
        <div class="embed constrain-width">
          <div class="horizontal-line" />
          <div class="embed-body">
            <PitchBox
              readonly={true}
              submit_disabled={true}
              title={"Adding eval to rust"}
              explanation={
                'Embeding a minimal rust compiler into programs to dynamically compile code at runtime. The evaluated code can be used sandboxed, or interact with a state you define.\n\nThis is useful for games, instructional programs, and demos etc.\n\n```rust\nuse eval::eval;\n\nfn main() {\n\teval(\"println!(\\"Hello World!\\");");\n}\n```'
              }
            />
          </div>
        </div>

        <h3 class="lato-bold paragraph">Why are we voting on pitches?</h3>
        <div class="lato-regular paragraph">
          Pitches should be fun to talk about, vote on pitches you like, and
          give feed back to each pitch's slack thread.
        </div>

        <div class="embed">
          <div class="horizontal-line" />
          <div class="embed-body">
            <Voting
              readonly={true}
              posts={[
                {
                  name: "Adding eval to Rust",
                  points: 110,
                  author: "William Daniel",
                  slackDiscusion: "about:blank",
                  id: 0,
                },
                {
                  name: "Doom in a PDF",
                  points: 90,
                  author: "vk6",
                  slackDiscusion: "about:blank",
                  id: 0,
                },
                {
                  name: "Hexecute: Launch apps by casting spells!",
                  points: 80,
                  author: "Andromeda",
                  slackDiscusion: "about:blank",
                  id: 0,
                },
                {
                  name: "Porting Celeste (2018) to the browser",
                  points: 70,
                  author: "r58",
                  slackDiscusion: "about:blank",
                  id: 0,
                },
              ]}
              loading={false}
            />
          </div>
        </div>

        <div class="lato-bold paragraph">How do I start?</div>

        <div class="lato-regular paragraph">
          Register, if this sounds interesting to you. If you don't have a Hack
          Club account yet, you will need to make one.
        </div>

        <div class="rsvp-container">
          <input placeholder="name@domain.tld" />
          <button
            class="rsvp"
            on:click={() => window.location.assign("/auth/login")}
          >
            Register an account!
          </button>
        </div>

        <h3 class="lato-bold paragraph">Where's the money coming from? </h3>
        <div class="lato-regular paragraph">
          <a href="https://hackclub.com/">HackClub</a> is a 501(c)(3) nonprofit
          supported by organizations like Github, AMD, OpenSauce, Girls Who
          Code, MIT, and more. The goal is to get more teens into engineering,
          and teens {"(<18)"} to get good at engineering.
        </div>
      </div>
    </div>
  );
}
Homepage.style = css`
  .rsvp {
    height: 30px;
  }

  .rsvp-container {
    display: flex;
    justify-content: center;
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
    grid-template-columns: 25% 25% 25% auto;
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
    grid-template-columns: 25% 25% 25% auto;
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
  .prizes {
    display: grid;
    grid-template-columns: auto auto auto auto;
    grid-auto-flow: row;
    align-items: center;
  }

  .prize-label {
    font-weight: bold;
    font-size: larger;
    padding: 8px;
  }

  .prize-item {
    display: flex;
    flex-direction: column;

    align-items: center;
    justify-content: center;
    gap: 8px;
    padding: 8px;
  }

  .prize-img {
    display: block;
    width: 20vh;
    height: 20vh;
    object-fit: contain;
  }

  .paragraph {
    font-size: large;
    margin-top:10px;
    margin-bottom:10px;
  }

  .small-text {
    font-size: small;
    margin-top: 50px;
    margin-bottom: 100px;
  }

  .copy {
    margin-bottom: 100px;
  }

  .big-text {
    font-size: xx-large;
  }

`;
