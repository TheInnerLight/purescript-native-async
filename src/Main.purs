module Main where

import Effect
import Prelude

import Effect.Async (liftEffectA, parTraverseA, runA, waitA)
import Effect.Console (log)

main :: Effect Unit
main = runA $ do
  _ <- liftEffectA $ log $ show "Test"
  x <- parTraverseA [(waitA 50), (waitA 500), (waitA 300)] (\x -> pure "Hello!")
  _ <- waitA 4
  liftEffectA $ log ("Hello World " <> show x)
  