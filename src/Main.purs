module Main where

import Effect
import Prelude

import Control.Parallel (parallel, sequential)
import Data.Traversable (sequence)
import Effect.Async (runSynchronously, wait)
import Effect.Class (liftEffect)
import Effect.Console (log)

main :: Effect Unit
main = runSynchronously $ do
  _ <- liftEffect $ log $ show "Test"
  x <- sequential $ sequence [parallel (wait 50), parallel (wait 500), parallel (wait 300)]
  y <- sequence [(wait 50), (wait 500), (wait 300)]
  _ <- liftEffect $ log ("Hello World " <> show x)
  liftEffect $ log ("Hello World " <> show y)
  