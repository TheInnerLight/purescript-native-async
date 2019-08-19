module Main where

import Effect
import Prelude

import Effect.Async (pureA, runA, bindA, waitA)
import Effect.Console (log)

main :: Effect Unit
main = do
  x <- runA $ bindA (waitA) (\x -> waitA)
  log ("Hello World " <> show x)
  