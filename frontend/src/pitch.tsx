import DOMPurify from "isomorphic-dompurify";
import { css, type FC } from "dreamland/core";
import { Marked } from "marked";
import { markedHighlight } from "marked-highlight";
import hljs from "highlight.js";
import "highlight.js/styles/stackoverflow-light.css";
import { PalatineHeader } from "./voting";

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

export function PitchBox(
  this: FC<
    {
      readonly: boolean;
      submit_disabled: boolean;
      title?: string;
      explanation?: string;
      pitching?: () => void;
    },
    {
      access_token: string;
    }
  >,
) {
  if (!this.title) {
    this.title = "";
  }
  if (!this.explanation) {
    this.explanation = "";
  }

  const update_md = () => {
    if (DOMPurify.sanitize) {
      const parsed = marked.parse(this.explanation!);
      const preview = this.root.querySelector("#preview");
      if (preview) {
        preview.innerHTML = DOMPurify.sanitize(parsed.toString());
      }
    }
  };

  this.cx.mount = () => {
    this.access_token = localStorage["access_token"];
    update_md();
  };

  return (
    <div>
      <PalatineHeader clickable={this.readonly ? false : true}>
        Pitch
      </PalatineHeader>
      <div class="lato-bold content">
        <form
          class="form"
          method="post"
          action="/api/v1/pitch_submit"
          on:submit={() => {
            this.pitching ? this.pitching() : null;
          }}
        >
          <div class="form-body">
            <input
              style={{ display: "none" }}
              name="access_token"
              value={use(this.access_token)}
            />
            <label for="pitch-title">title</label>
            <input
              type="text"
              id="pitch-name"
              name="title"
              readonly={this.readonly}
              value={use(this.title)}
              maxlength="256"
              required
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
              maxlength="12000"
              on:input={(e: InputEvent) => {
                this.explanation = (e.target as HTMLTextAreaElement).value;
                update_md();
              }}
              required
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
