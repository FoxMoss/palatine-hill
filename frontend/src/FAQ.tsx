import { type FC } from "dreamland/core";
import { PalatineHeader } from "./voting";

export function FAQ(this: FC<{}, {}>) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
  };

  return (
    <div class="fullscreen-container">
      <PalatineHeader clickable={true}>FAQ</PalatineHeader>
      <div class="lato-regular content">
        <PitchExplainer/>
      </div>
    </div>
  );
}

export function PitchExplainer(this: FC<{}, {}>) {
  return (
    <>
      <h1>How do pitches work?</h1>
      <p>
        Pitches are pretty simple, write out a paragraph about your pitch with
        explanatory title. This pitch cannot be deleted without DMing me (@fox
        ellison-taylor) on Slack so make sure its polished. Once posted
        it'll create a slack thread in{" "}
        <a href="https://hackclub.enterprise.slack.com/archives/C0B697HHCE6">
          #palatine-hill-threads
        </a>
        . See the example below:
      </p>
      <p>
        <img src="/threadexample.png" alt="thread example" />
      </p>
      <p>
        All likes will get added as pings in Slack so you get notified on
        updates to the project. Discuss updates, or ask questions about
        execution :)
      </p>
      <p>
        <img src="/likeexample.png" alt="like example" />
      </p>
      <p>
        After the deadline pitch submissions will be locked and you can stay
        active in the slack while you work on your project!
      </p>
    </>
  );
}
