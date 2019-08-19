module Main where

import Effect
import Prelude

import Effect.Async (liftEffectA, parTraverseA, runA, waitA)
import Effect.Console (log)

main :: Effect Unit
main = runA $ do
  _ <- liftEffectA $ log $ show "d"
  x <- parTraverseA [waitA, waitA, waitA] (\x -> pure 1)
  --_ <- waitA
  liftEffectA $ log ("Hello World " <> show x)
  