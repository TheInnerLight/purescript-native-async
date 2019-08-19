module Main where

import Effect
import Prelude

import Effect.Async (runA, waitA, liftEffectA)
import Effect.Console (log)

main :: Effect Unit
main = runA $ do
  _ <- waitA
  _ <- liftEffectA $ log $ show "d"
  x <- waitA
  _ <- waitA
  _ <- waitA
  liftEffectA $ log ("Hello World " <> show x)
  