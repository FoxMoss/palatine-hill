import { type FC } from "dreamland/core";
import { Post, Voting } from "./voting";

export function Dashboard(this: FC<{}, { user: String; posts: Post[] | undefined }>) {
  this.cx.mount = () => {
    if (!localStorage["access_token"]) {
      window.location.assign("/auth/login");
    }
    this.user = localStorage["name"];
  };

  return <Voting readonly={false} posts={use(this.posts)} loading={true} />;
}
