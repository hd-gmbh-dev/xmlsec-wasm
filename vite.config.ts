import { fileURLToPath, URL } from "node:url"

/// <reference types="vitest" />
import { defineConfig } from "vite"
import type { UserConfig as VitestUserConfigInterface } from "vitest/config"

const vitestConfig: VitestUserConfigInterface = {
  test: {
    globals: true,    
    environment: "node",
  },
}

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./src", import.meta.url)),
    },
  },
  test: vitestConfig.test,
})