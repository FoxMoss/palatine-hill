import { css, type FC } from "dreamland/core";
export default function ErrorPage(this: FC<{}, {}>) {
  return (
    <div>
      <div class="container">
        <div class="paragraph">
          <h1 class="title lato-black">Palatine Hill</h1>
          <div class="border" />
        </div>
        <h3 class="lato-bold paragraph">404 hill not found.</h3>
      </div>
    </div>
  );
}
ErrorPage.style = css`
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
