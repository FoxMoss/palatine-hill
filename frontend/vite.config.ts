import { defineConfig } from "vite";
import { cssMinifier, devSsr } from "dreamland/vite";

export default defineConfig({
  build: {
    rolldownOptions: {
      input: ["/index.html"],
      output: {
        codeSplitting: true,
      },
    },
  },
  plugins: [
    cssMinifier({
			include: ["src/**/*.tsx"],
		}),
    devSsr({
      entry: "/src/main-server.ts",
    }),
  ],
});
